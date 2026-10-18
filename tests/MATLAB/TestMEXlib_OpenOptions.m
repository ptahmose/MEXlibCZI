classdef TestMEXlib_OpenOptions < matlab.unittest.TestCase
    methods (Test)
        function testXtSubblocksWorkaround(testCase)
            % This test verifies the XtSubblocks dimension-swap workaround.
            %
            % The test file Xttest.czi contains a single subblock with an
            % unconventional dimension layout. Without the workaround,
            % the subblock's bounding box is reported as 3x1 pixels.
            %
            % Enabling "dimensionSwapWorkaround=XtSubblocks" causes the
            % subblock to be interpreted differently, resulting in a
            % bounding box of 3x4 pixels.
            %
            % The number of subblocks must remain unchanged.

            % Locate the test file relative to this unit test.
            testDir = fileparts(mfilename('fullpath'));
            filename = fullfile(testDir, 'Xttest.czi');

            testCase.assertTrue(isfile(filename), ...
                'Test file Xttest.czi not found.');

            % First, open the file normally, without any workaround.
            % The expected bounding box is X=0, Y=0, W=3, H=1.
            handle = MEXlibCZI('Open', filename);
            cleanupReader = onCleanup(@() MEXlibCZI('Close', handle));

            info = MEXlibCZI('GetInfo', handle);

            testCase.verifyEqual(info.subblockcount, int32(1));
            testCase.verifyEqual(info.boundingBox, int32([0 0 3 1]));

            clear cleanupReader;

            % Now open the same file with the XtSubblocks workaround.
            % The workaround should correct the interpretation of the
            % subblock dimensions, changing the bounding box to 3x4.
            %
            % Note that the underlying CZI file is not modified.
            handle = MEXlibCZI('Open', filename, ...
                'dimensionSwapWorkaround=XtSubblocks');
            cleanupReader = onCleanup(@() MEXlibCZI('Close', handle));

            info = MEXlibCZI('GetInfo', handle);

            % Verify that the subblock count is unchanged, while the
            % bounding box reflects the corrected dimensions.
            testCase.verifyEqual(info.subblockcount, int32(1));
            testCase.verifyEqual(info.boundingBox, int32([0 0 3 4]));

            clear cleanupReader;
        end

        function testXzSubblocksWorkaround(testCase)
            % This test verifies the XzSubblocks dimension-swap workaround.
            %
            % The test file Xztest.czi contains a subblock with an
            % unconventional dimension layout.
            %
            % Without the workaround, the bounding box reflects the
            % original interpretation of the subblock dimensions.
            %
            % Enabling "dimensionSwapWorkaround=XzSubblocks" changes the
            % interpretation of the subblock dimensions, resulting in
            % a different bounding box.
            %
            % The underlying CZI file is not modified.
        
            % Locate the test file relative to this unit test.
            testDir = fileparts(mfilename('fullpath'));
            filename = fullfile(testDir, 'Xztest.czi');
        
            testCase.assertTrue(isfile(filename), ...
                'Test file Xztest.czi not found.');
        
            % First, open the file normally, without any workaround.
            handle = MEXlibCZI('Open', filename);
            cleanupReader = onCleanup(@() MEXlibCZI('Close', handle));
        
            info = MEXlibCZI('GetInfo', handle);
        
            % TODO: Verify the expected statistics without the workaround.
            % testCase.verifyEqual(info.subblockcount, int32(1));
            % testCase.verifyEqual(info.boundingBox, int32([0 0 3 1]));
        
            clear cleanupReader;
        
            % Now open the same file with the XzSubblocks workaround enabled.
            handle = MEXlibCZI('Open', filename, ...
                'dimensionSwapWorkaround=XzSubblocks');
            cleanupReader = onCleanup(@() MEXlibCZI('Close', handle));
        
            info = MEXlibCZI('GetInfo', handle);
        
            % TODO: Verify the expected statistics with the workaround.
            % testCase.verifyEqual(info.subblockcount, int32(1));
            % testCase.verifyEqual(info.boundingBox, int32([0 0 3 4]));
        
            clear cleanupReader;
        end
    end
end
