% File: test_write.m

%!test
%! filename = [tempname() '.czi'];
%! writer = octavelibczi('CreateCziWriter', filename, 'x');
%! octavelibczi('AddSubBlock', writer, 'C0', [0 0 2 2], 'gray8', uint8([1 2; 3 4]), struct('M', 0));
%! metadata_builder = octavelibczi('Writer_GetPreparedMetadata', writer);
%! octavelibczi('MetadataBuilder_Modify', metadata_builder, 'Scaling', struct('X', 0.25e-6, 'Y', 0.5e-6));
%! octavelibczi('MetadataBuilder_Modify', metadata_builder, 'Scaling', struct('Z', 1e-6));
%! xml = octavelibczi('MetadataBuilder_GetXml', metadata_builder);
%! octavelibczi('MetadataBuilder_SetXml', metadata_builder, xml);
%! octavelibczi('Writer_WriteMetadata', writer, xml);
%! octavelibczi('CloseCziWriter', writer);
%! octavelibczi('MetadataBuilder_Release', metadata_builder);
%! reader = octavelibczi('Open', filename);
%! scaling = octavelibczi('GetScaling', reader);
%! assert([scaling.scaleX scaling.scaleY scaling.scaleZ], [0.25e-6 0.5e-6 1e-6], 1e-15);
%! octavelibczi('Close', reader);
%! delete(filename);


%!test
%! handle=octavelibczi ('CreateCziWriter','test.czi','x');
%! uint8_array = uint8([1, 2; 3, 4]);
%! octavelibczi ('AddSubBlock',handle,'C0T0',[0 0 2 2],'gray8',uint8_array,struct('M',0));
%! uint8_array = uint8([10, 20; 30, 40]);
%! octavelibczi ('AddSubBlock',handle,'C0T1',[0 0 2 2],'gray8',uint8_array,struct('M',0));
%! octavelibczi ('CloseCziWriter',handle);
%! handle = octavelibczi ('Open','test.czi');
%! info = octavelibczi ('GetInfo', handle);
%! assert(isa(info.subblockcount, 'int32'));
%! assert(info.subblockcount, int32(2));
%! assert(info.minMindex, int32(0));
%! assert(info.maxMindex, int32(0));
%! assert(info.boundingBox, int32([0,0,2,2]));
%! assert(info.boundingBoxLayer0, int32([0,0,2,2]));
%! octavelibczi ('Close',handle);
%! delete('test.czi');

%!test
%! handle=octavelibczi ('CreateCziWriter','test2.czi','x');
%! uint8_array = uint8([1, 2; 3, 4]);
%! octavelibczi ('AddSubBlock',handle,'C0T0',[0 0 2 2],'gray8',uint8_array,struct('M',0));
%! uint8_array = uint8([10, 20; 30, 40]);
%! octavelibczi ('AddSubBlock',handle,'C0T1',[0 0 2 2],'gray8',uint8_array,struct('M',0));
%! octavelibczi ('CloseCziWriter',handle);
%! handle = octavelibczi ('Open','test2.czi');
%! subblock1 = octavelibczi ('GetSubBlockBitmap', handle, 0);
%! assert(subblock1, uint8([1, 2; 3, 4]));
%! subblock2 = octavelibczi ('GetSubBlockBitmap', handle, 1);
%! assert(subblock2, uint8([10, 20; 30, 40]));
%! octavelibczi ('Close',handle);
%! delete('test2.czi');
