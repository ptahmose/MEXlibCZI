% File: test_write.m

%!test
%! filename = [tempname() '.czi'];
%! writer = octavelibczi('CreateCziWriter', filename, 'x');
%! octavelibczi('AddSubBlock', writer, 'C0', [0 0 2 2], 'gray8', uint8([1 2; 3 4]), struct('M', 0));
%! metadata_builder = octavelibczi('Writer_GetPreparedMetadata', writer);
%! info = struct('Name', 'Processed & checked', 'Title', 'Sample', 'UserName', 'Tester', ...
%!     'Description', 'Background subtraction', 'Comment', 'Completed', ...
%!     'Keywords', 'processed;fluorescence', 'CreationDateTime', '2026-10-29T13:42:17.123Z', 'Rating', int32(5));
%! octavelibczi('MetadataBuilder_Modify', metadata_builder, 'GeneralDocumentInfo', info);
%! original = octavelibczi('MetadataBuilder_GetXml', metadata_builder);
%! assert(!isempty(strfind(original, '<Rating>5</Rating>')));
%! octavelibczi('MetadataBuilder_Modify', metadata_builder, 'GeneralDocumentInfo', struct('Name', ''));
%! xml = octavelibczi('MetadataBuilder_GetXml', metadata_builder);
%! assert(xml, original);
%! % Updating one field preserves all omitted fields.
%! octavelibczi('MetadataBuilder_Modify', metadata_builder, 'GeneralDocumentInfo', struct('Rating', int32(0)));
%! xml = octavelibczi('MetadataBuilder_GetXml', metadata_builder);
%! octavelibczi('Writer_WriteMetadata', writer, xml);
%! octavelibczi('CloseCziWriter', writer);
%! octavelibczi('MetadataBuilder_Release', metadata_builder);
%! reader = octavelibczi('Open', filename);
%! xml = octavelibczi('GetMetadataXml', reader);
%! tags = {'Name', 'Title', 'UserName', 'Description', 'Comment', 'Keywords', 'CreationDate', 'Rating'};
%! expected = {'Processed &amp; checked', 'Sample', 'Tester', 'Background subtraction', ...
%!     'Completed', 'processed;fluorescence', '2026-10-29T13:42:17.123Z', '0'};
%! for k = 1:numel(tags)
%!     assert(!isempty(strfind(xml, ['<' tags{k} '>' expected{k} '</' tags{k} '>'])));
%! end
%! octavelibczi('Close', reader);
%! delete(filename);

%!test
%! filename = [tempname() '.czi'];
%! writer = octavelibczi('CreateCziWriter', filename, 'x');
%! octavelibczi('AddSubBlock', writer, 'C0', [0 0 2 2], 'gray8', uint8([1 2; 3 4]), struct('M', 0));
%! metadata_builder = octavelibczi('Writer_GetPreparedMetadata', writer);
%! octavelibczi('CloseCziWriter', writer);
%! octavelibczi('MetadataBuilder_Modify', metadata_builder, 'GeneralDocumentInfo', struct('Name', 'Original'));
%! original = octavelibczi('MetadataBuilder_GetXml', metadata_builder);
%! invalid_values = {struct('Name', 'must not change', 'Comment', 123), struct(), struct('Unknown', 'value')};
%! for rating = {int32(-1), int32(6), int32([1 2]), 4, 1.5}
%!     invalid_values{end + 1} = struct('Name', 'must not change', 'Rating', rating{1});
%! end
%! for values = invalid_values
%!     rejected = false;
%!     try
%!         octavelibczi('MetadataBuilder_Modify', metadata_builder, 'GeneralDocumentInfo', values{1});
%!     catch exception
%!         assert(exception.identifier, 'MEXlibCZI:InvalidArguments');
%!         rejected = true;
%!     end
%!     assert(rejected);
%!     xml = octavelibczi('MetadataBuilder_GetXml', metadata_builder);
%!     assert(xml, original);
%! end
%! octavelibczi('MetadataBuilder_Release', metadata_builder);
%! delete(filename);

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
