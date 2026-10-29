classdef TestMEXlib_MetadataBuilder < matlab.unittest.TestCase
    methods (Test)
        function roundTripScalingAndXml(testCase)
            filename = [tempname '.czi'];
            testCase.addTeardown(@() delete(filename));
            writer = MEXlibCZI('CreateCziWriter', filename, 'x');
            MEXlibCZI('AddSubBlock', writer, 'C0', [0 0 2 2], ...
                'gray8', uint8([1 2; 3 4]), struct('M', 0));
            metadata_builder = MEXlibCZI('Writer_GetPreparedMetadata', writer);
            MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'Scaling', ...
                struct('X', 0.25e-6, 'Y', 0.5e-6, 'DefaultUnitFormatX', 'um'));
            MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'Scaling', struct('Z', 1e-6));
            xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
            xml = strrep(xml, '</Metadata>', ...
                '<CustomAttributes><DraftMarker>preserved</DraftMarker></CustomAttributes></Metadata>');
            MEXlibCZI('MetadataBuilder_SetXml', metadata_builder, xml);
            xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
            testCase.verifyTrue(contains(xml, 'DraftMarker'));
            testCase.verifyTrue(contains(xml, '<DefaultUnitFormat>um</DefaultUnitFormat>'));
            MEXlibCZI('Writer_WriteMetadata', writer, xml);
            MEXlibCZI('CloseCziWriter', writer);
            % Builders remain usable independently of their original writer.
            xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
            testCase.verifyTrue(contains(xml, 'DraftMarker'));
            MEXlibCZI('MetadataBuilder_Release', metadata_builder);
            testCase.verifyError(@() MEXlibCZI('MetadataBuilder_Release', metadata_builder), ...
                'MEXlibCZI:InvalidArguments');
            reader = MEXlibCZI('Open', filename);
            scaling = MEXlibCZI('GetScaling', reader);
            testCase.verifyEqual([scaling.scaleX scaling.scaleY scaling.scaleZ], ...
                [0.25e-6 0.5e-6 1e-6], 'RelTol', 1e-12);
            xml = MEXlibCZI('GetMetadataXml', reader);
            testCase.verifyTrue(contains(xml, 'DraftMarker'));
            MEXlibCZI('Close', reader);
        end

        function preparationAloneKeepsAutomaticMetadata(testCase)
            filename = [tempname '.czi'];
            testCase.addTeardown(@() delete(filename));
            writer = MEXlibCZI('CreateCziWriter', filename, 'x');
            MEXlibCZI('AddSubBlock', writer, 'C0', [0 0 2 2], ...
                'gray8', uint8([1 2; 3 4]), struct('M', 0));
            metadata_builder = MEXlibCZI('Writer_GetPreparedMetadata', writer);
            original = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
            for value = {0, -1, NaN, Inf, [1 2], 'bad'}
                testCase.verifyError(@() MEXlibCZI('MetadataBuilder_Modify', metadata_builder, ...
                    'Scaling', struct('X', value{1})), 'MEXlibCZI:InvalidArguments');
            end
            xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
            testCase.verifyEqual(xml, original);
            MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'Scaling', struct('X', 1e-6));
            testCase.verifyError(@() MEXlibCZI('Writer_WriteMetadata', writer, '<broken'), ...
                'MEXlibCZI:InvalidArguments');
            MEXlibCZI('CloseCziWriter', writer);
            MEXlibCZI('MetadataBuilder_Release', metadata_builder);
            reader = MEXlibCZI('Open', filename);
            scaling = MEXlibCZI('GetScaling', reader);
            testCase.verifyTrue(isnan(scaling.scaleX));
            xml = MEXlibCZI('GetMetadataXml', reader);
            testCase.verifyTrue(contains(xml, 'DisplaySetting'));
            MEXlibCZI('Close', reader);
        end
    end
end
