classdef TestMEXlib_OpenOptions < matlab.unittest.TestCase
    methods (Test)
        function testOptionalOpenOptions(testCase)
            filename = [tempname '.czi'];
            cleanupFile = onCleanup(@() delete(filename));
            writer = MEXlibCZI('CreateCziWriter', filename, 'x');
            MEXlibCZI('AddSubBlock', writer, 'C0T0', [0 0 2 2], ...
                'gray8', uint8([1 2; 3 4]), struct('M', 0));
            MEXlibCZI('CloseCziWriter', writer);

            variants = {{}, {''}, ...
                {'laxSubblockCoordinateChecks=false;swapTAndY=true;subBlockDirectoryInfoDiscrepancy=ignore'}};
            for i = 1:numel(variants)
                args = variants{i};
                handle = MEXlibCZI('Open', filename, args{:});
                cleanupReader = onCleanup(@() MEXlibCZI('Close', handle));
                info = MEXlibCZI('GetInfo', handle);
                testCase.verifyEqual(info.subblockcount, int32(1));
                clear cleanupReader;
            end

            badOptions = {42, 'laxSubblockCoordinateChecks=yes', 'missingEquals'};
            for i = 1:numel(badOptions)
                didThrow = false;
                try
                    handle = MEXlibCZI('Open', filename, badOptions{i});
                    MEXlibCZI('Close', handle);
                catch
                    didThrow = true;
                end
                testCase.verifyTrue(didThrow);
            end
        end
    end
end
