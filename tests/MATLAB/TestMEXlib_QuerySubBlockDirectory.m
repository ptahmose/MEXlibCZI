% File: TestMEXlib_QuerySubBlockDirectory.m
classdef TestMEXlib_QuerySubBlockDirectory < matlab.unittest.TestCase

    methods (Test)
        function testQueryAllSubBlocks(testCase)
            [handle, cleanupFile] = createTestFile('test_qsbd_all.czi'); %#ok<ASGLU>
            cleanupReader = onCleanup(@() MEXlibCZI('Close', handle)); %#ok<NASGU>

            d = MEXlibCZI('QuerySubBlockDirectory', handle);

            testCase.verifyClass(d.Index, 'int32');
            testCase.verifyClass(d.Mode, 'uint8');
            testCase.verifyClass(d.Pixeltype, 'uint8');
            testCase.verifyClass(d.Coordinate, 'int32');
            testCase.verifyClass(d.LogicalRect, 'int32');
            testCase.verifyClass(d.PhysicalSize, 'int32');
            testCase.verifyClass(d.MIndex, 'int32');
            testCase.verifyEqual(d.CoordinateDimensions, 'ZCT');
            testCase.verifyEqual(size(d.Index), [5 1]);
            testCase.verifyEqual(size(d.Coordinate), [5 3]);
            testCase.verifyEqual(size(d.LogicalRect), [5 4]);
            testCase.verifyEqual(size(d.PhysicalSize), [5 2]);
            testCase.verifyEqual(sort(d.Index), int32((0:4)'));
            testCase.verifyEqual(d.Pixeltype, zeros(5, 1, 'uint8'));   % Gray8
            testCase.verifyEqual(d.Mode, zeros(5, 1, 'uint8'));        % uncompressed

            % the order is unspecified, so look up the subblock with T=5 by its coordinate
            row = find(d.Coordinate(:, 3) == 5);
            testCase.verifyEqual(numel(row), 1);
            testCase.verifyEqual(d.Coordinate(row, :), int32([intmin('int32') 0 5]));
            testCase.verifyEqual(d.LogicalRect(row, :), int32([10 10 2 2]));
            testCase.verifyEqual(d.PhysicalSize(row, :), int32([2 2]));
            testCase.verifyEqual(d.MIndex(row), int32(1));
        end

        function testQueryWithCoordinateFilter(testCase)
            [handle, cleanupFile] = createTestFile('test_qsbd_coord.czi'); %#ok<ASGLU>
            cleanupReader = onCleanup(@() MEXlibCZI('Close', handle)); %#ok<NASGU>

            d = MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', 1)));
            testCase.verifyEqual(d.CoordinateDimensions, 'ZC');
            testCase.verifyEqual(size(d.Index), [2 1]);
            testCase.verifyEqual(sortrows(d.Coordinate), int32([0 1; 1 1]));

            d = MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('Z', [0 2], 'C', 0)));
            testCase.verifyEqual(size(d.Index), [2 1]);
            testCase.verifyEqual(sortrows(d.Coordinate), int32([0 0; 1 0]));

            % an interval not starting at zero
            d = MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('Z', [1 5], 'C', [0 1])));
            testCase.verifyEqual(size(d.Index), [1 1]);
            testCase.verifyEqual(d.Coordinate, int32([1 0]));

            % the returned index can be used to retrieve the bitmap
            d = MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('Z', 1, 'C', 1)));
            testCase.verifyEqual(size(d.Index), [1 1]);
            bitmap = MEXlibCZI('GetSubBlockBitmap', handle, d.Index(1));
            testCase.verifyEqual(bitmap, uint8([4 4; 4 4]));
        end

        function testQueryWithRoiFilter(testCase)
            [handle, cleanupFile] = createTestFile('test_qsbd_roi.czi'); %#ok<ASGLU>
            cleanupReader = onCleanup(@() MEXlibCZI('Close', handle)); %#ok<NASGU>

            d = MEXlibCZI('QuerySubBlockDirectory', handle, struct('Roi', [9 9 5 5]));
            testCase.verifyEqual(size(d.Index), [1 1]);
            testCase.verifyEqual(d.LogicalRect, int32([10 10 2 2]));
        end

        function testQueryWithOnlyLayer0Filter(testCase)
            [handle, cleanupFile] = createTestFile('test_qsbd_layer0.czi'); %#ok<ASGLU>
            cleanupReader = onCleanup(@() MEXlibCZI('Close', handle)); %#ok<NASGU>

            d = MEXlibCZI('QuerySubBlockDirectory', handle, struct('OnlyLayer0', true));
            testCase.verifyEqual(size(d.Index), [5 1]);
        end

        function testQueryWithNoMatchReturnsEmptyArrays(testCase)
            [handle, cleanupFile] = createTestFile('test_qsbd_empty.czi'); %#ok<ASGLU>
            cleanupReader = onCleanup(@() MEXlibCZI('Close', handle)); %#ok<NASGU>

            d = MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', 42)));
            testCase.verifyEmpty(d.Index);
            testCase.verifyEqual(size(d.Coordinate, 1), 0);
            testCase.verifyEmpty(d.CoordinateDimensions);
        end

        function testInvalidArgumentsThrow(testCase)
            [handle, cleanupFile] = createTestFile('test_qsbd_args.czi'); %#ok<ASGLU>
            cleanupReader = onCleanup(@() MEXlibCZI('Close', handle)); %#ok<NASGU>

            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, 42), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('Roi', [1 2])), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', 1)), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', sparse([0 1])))), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('Roi', sparse([9 9 5 5]))), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', 0.6))), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', single(0.5)))), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('Roi', [9.5 9 5 5])), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', [0 0]))), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', [0 1 2]))), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', single(2147483648)))), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('Roi', single([2147483648 0 1 1]))), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('OnlyLayer0', [1 0])), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', handle, struct('OnlyLayer0', sparse(1))), ?MException);
            testCase.verifyError(@() MEXlibCZI('QuerySubBlockDirectory', sparse(handle)), ?MException);
        end

        function testUint32CoordinateSetAndRoi(testCase)
            [handle, cleanupFile] = createTestFile('test_qsbd_uint32.czi'); %#ok<ASGLU>
            cleanupReader = onCleanup(@() MEXlibCZI('Close', handle)); %#ok<NASGU>

            d = MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('Z', uint32(0), 'C', uint32([0 2]))));
            testCase.verifyEqual(size(d.Index), [2 1]);
            testCase.verifyEqual(sortrows(d.Coordinate), int32([0 0; 0 1]));

            d = MEXlibCZI('QuerySubBlockDirectory', handle, struct('Roi', uint32([9 9 5 5])));
            testCase.verifyEqual(d.LogicalRect, int32([10 10 2 2]));
        end
    end
end

% Creates a CZI with 5 pyramid-layer-0 subblocks (Z0C0, Z0C1, Z1C0, Z1C1 at [0 0 2 2] with
% pixel values 1..4, and C0T5 at [10 10 2 2]) and opens it.
function [handle, cleanupFile] = createTestFile(filename)
    writer = MEXlibCZI('CreateCziWriter', filename, 'x');
    MEXlibCZI('AddSubBlock', writer, 'Z0C0', [0 0 2 2], 'gray8', uint8(ones(2, 2) * 1), struct('M', 0));
    MEXlibCZI('AddSubBlock', writer, 'Z0C1', [0 0 2 2], 'gray8', uint8(ones(2, 2) * 2), struct('M', 0));
    MEXlibCZI('AddSubBlock', writer, 'Z1C0', [0 0 2 2], 'gray8', uint8(ones(2, 2) * 3), struct('M', 0));
    MEXlibCZI('AddSubBlock', writer, 'Z1C1', [0 0 2 2], 'gray8', uint8(ones(2, 2) * 4), struct('M', 0));
    MEXlibCZI('AddSubBlock', writer, 'C0T5', [10 10 2 2], 'gray8', uint8(ones(2, 2) * 5), struct('M', 1));
    MEXlibCZI('CloseCziWriter', writer);
    handle = MEXlibCZI('Open', filename);
    cleanupFile = onCleanup(@() delete(filename));
end
