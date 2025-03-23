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

        function testWriteMultiChannelCZIAndOpenItAndCheck(testCase)
            handle=MEXlibCZI ('CreateCziWriter','test_multichannel_8ch.czi','x');
            MEXlibCZI('AddSubBlock', handle, 'C00',[0,0,1024,768], 'gray8', uint8(randi([0,255], 1024, 768)), struct('M',0))
            MEXlibCZI('AddSubBlock', handle, 'C01',[0,0,1024,768], 'gray8', uint8(randi([0,255], 1024, 768)), struct('M',0))
            MEXlibCZI('AddSubBlock', handle, 'C02',[0,0,1024,768], 'gray8', uint8(randi([0,255], 1024, 768)), struct('M',0))
            MEXlibCZI('AddSubBlock', handle, 'C03',[0,0,1024,768], 'gray8', uint8(randi([0,255], 1024, 768)), struct('M',0))
            MEXlibCZI('AddSubBlock', handle, 'C04',[0,0,1024,768], 'gray8', uint8(randi([0,255], 1024, 768)), struct('M',0))
            MEXlibCZI('AddSubBlock', handle, 'C05',[0,0,1024,768], 'gray8', uint8(randi([0,255], 1024, 768)), struct('M',0))
            MEXlibCZI('AddSubBlock', handle, 'C06',[0,0,1024,768], 'gray8', uint8(randi([0,255], 1024, 768)), struct('M',0))
            MEXlibCZI('AddSubBlock', handle, 'C07',[0,0,1024,768], 'gray8', uint8(randi([0,255], 1024, 768)), struct('M',0))
            MEXlibCZI ('CloseCziWriter',handle);
            handle = MEXlibCZI ('Open','test_multichannel_8ch.czi');
            info = MEXlibCZI ('GetInfo', handle);
            testCase.verifyTrue(isa(info.subblockcount, 'int32'));
            testCase.verifyEqual(info.subblockcount, int32(8));
            testCase.verifyClass(info.dimBounds.C, 'int32');
            testCase.verifyEqual(info.dimBounds.C, int32([0, 8]));
            MEXlibCZI ('Close',handle);
            delete('test_multichannel_8ch.czi');
        end
    end
end