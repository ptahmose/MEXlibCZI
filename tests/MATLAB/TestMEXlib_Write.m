% File: TestMEXlib_Write.m
classdef TestMEXlib_Write < matlab.unittest.TestCase

    methods (Test)
        function testWriteCZIAndOpenItAndCheckInfo(testCase)
            handle=MEXlibCZI ('CreateCziWriter','test.czi','x');
            uint8_array = uint8([1, 2; 3, 4]);
            MEXlibCZI ('AddSubBlock',handle,'C0T0',[0 0 2 2],'gray8',uint8_array,struct('M',0));
            uint8_array = uint8([10, 20; 30, 40]);
            MEXlibCZI ('AddSubBlock',handle,'C0T1',[0 0 2 2],'gray8',uint8_array,struct('M',0));
            MEXlibCZI ('CloseCziWriter',handle);
            handle = MEXlibCZI ('Open','test.czi');
            info = MEXlibCZI ('GetInfo', handle);
            testCase.verifyTrue(isa(info.subblockcount, 'int32'));
            testCase.verifyEqual(info.subblockcount, int32(2));
            testCase.verifyEqual(info.minMindex, int32(0));
            testCase.verifyEqual(info.maxMindex, int32(0));
            testCase.verifyEqual(info.boundingBox, int32([0,0,2,2]));
            testCase.verifyEqual(info.boundingBoxLayer0, int32([0,0,2,2]));
            MEXlibCZI ('Close',handle);
            delete('test.czi');
        end

        function testWriteCZIAndOpenItAndReadSubBlocksAndCheckThem(testCase)
            handle=MEXlibCZI ('CreateCziWriter','test2.czi','x');
            uint8_array = uint8([1, 2; 3, 4]);
            MEXlibCZI ('AddSubBlock',handle,'C0T0',[0 0 2 2],'gray8',uint8_array,struct('M',0));
            uint8_array = uint8([10, 20; 30, 40]);
            MEXlibCZI ('AddSubBlock',handle,'C0T1',[0 0 2 2],'gray8',uint8_array,struct('M',0));
            MEXlibCZI ('CloseCziWriter',handle);
            handle = MEXlibCZI ('Open','test2.czi');
            subblock1 = MEXlibCZI ('GetSubBlockBitmap', handle, 0);
            testCase.verifyEqual(subblock1, uint8([1, 2; 3, 4]));
            subblock2 = MEXlibCZI ('GetSubBlockBitmap', handle, 1);
            testCase.verifyEqual(subblock2, uint8([10, 20; 30, 40]));
            MEXlibCZI ('Close',handle);
            delete('test2.czi');
        end
    end
end