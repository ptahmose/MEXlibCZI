classdef TestMEXlib_MetadataBuilder < matlab.unittest.TestCase
    methods (Test)
        function roundTripGeneralDocumentInfo(testCase)
            filename = [tempname '.czi'];
            testCase.addTeardown(@() delete(filename));
            writer = MEXlibCZI('CreateCziWriter', filename, 'x');
            MEXlibCZI('AddSubBlock', writer, 'C0', [0 0 2 2], ...
                'gray8', uint8([1 2; 3 4]), struct('M', 0));
            metadata_builder = MEXlibCZI('Writer_GetPreparedMetadata', writer);
            testCase.addTeardown(@() MEXlibCZI('MetadataBuilder_Release', metadata_builder));

            t = datetime(2026, 10, 29, 13, 42, 17.123, 'TimeZone', 'UTC');
            t.Format = 'yyyy-MM-dd''T''HH:mm:ss.SSSXXX';
            info = struct('Name', 'Processed & checked', 'Title', 'Sample', ...
                'UserName', 'Tester', 'Description', 'Background subtraction', ...
                'Comment', 'Completed', 'Keywords', 'processed;fluorescence', ...
                'CreationDateTime', char(t), 'Rating', int32(5));
            MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'GeneralDocumentInfo', info);
            original = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);

            % Empty text must preserve the existing value, matching libCZI setters.
            MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'GeneralDocumentInfo', struct('Name', ''));
            xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
            testCase.verifyEqual(xml, original);
            testCase.verifyTrue(contains(xml, '<Rating>5</Rating>'));

            % Updating only the rating must preserve all omitted document fields.
            MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'GeneralDocumentInfo', struct('Rating', int32(0)));
            xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
            MEXlibCZI('Writer_WriteMetadata', writer, xml);
            MEXlibCZI('CloseCziWriter', writer);
            reader = MEXlibCZI('Open', filename);
            testCase.addTeardown(@() MEXlibCZI('Close', reader));
            xml = MEXlibCZI('GetMetadataXml', reader);

            % Check persisted values, including XML escaping and the CreationDate mapping.
            tags = {'Name', 'Title', 'UserName', 'Description', 'Comment', ...
                'Keywords', 'CreationDate', 'Rating'};
            expected = {'Processed &amp; checked', 'Sample', 'Tester', ...
                'Background subtraction', 'Completed', 'processed;fluorescence', ...
                '2026-10-29T13:42:17.123Z', '0'};
            for k = 1:numel(tags)
                testCase.verifyTrue(contains(xml, ['<' tags{k} '>' expected{k} '</' tags{k} '>']), tags{k});
            end
        end

        function invalidGeneralDocumentInfoPreservesBuilder(testCase)
            filename = [tempname '.czi'];
            testCase.addTeardown(@() delete(filename));
            writer = MEXlibCZI('CreateCziWriter', filename, 'x');
            MEXlibCZI('AddSubBlock', writer, 'C0', [0 0 2 2], ...
                'gray8', uint8([1 2; 3 4]), struct('M', 0));
            metadata_builder = MEXlibCZI('Writer_GetPreparedMetadata', writer);
            testCase.addTeardown(@() MEXlibCZI('MetadataBuilder_Release', metadata_builder));
            MEXlibCZI('CloseCziWriter', writer);
            MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'GeneralDocumentInfo', struct('Name', 'Original'));
            original = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);

            % A valid Name followed by an invalid Rating must not apply a partial update.
            for rating = {int32(-1), int32(6), int32([1 2]), 4, 1.5}
                testCase.verifyError(@() MEXlibCZI('MetadataBuilder_Modify', metadata_builder, ...
                    'GeneralDocumentInfo', struct('Name', 'must not change', 'Rating', rating{1})), ...
                    'MEXlibCZI:InvalidArguments');
                xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
                testCase.verifyEqual(xml, original);
            end

            for values = {struct('Name', 'must not change', 'Comment', 123), ...
                    struct('CreationDateTime', datetime(2026, 10, 29)), struct(), struct('Unknown', 'value')}
                testCase.verifyError(@() MEXlibCZI('MetadataBuilder_Modify', metadata_builder, ...
                    'GeneralDocumentInfo', values{1}), 'MEXlibCZI:InvalidArguments');
                xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
                testCase.verifyEqual(xml, original);
            end
        end

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
