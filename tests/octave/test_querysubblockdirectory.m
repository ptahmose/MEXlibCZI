% File: test_querysubblockdirectory.m

%!function handle = create_test_file(filename)
%!  writer = octavelibczi ('CreateCziWriter', filename, 'x');
%!  octavelibczi ('AddSubBlock', writer, 'Z0C0', [0 0 2 2], 'gray8', uint8(ones(2, 2) * 1), struct('M', 0));
%!  octavelibczi ('AddSubBlock', writer, 'Z0C1', [0 0 2 2], 'gray8', uint8(ones(2, 2) * 2), struct('M', 0));
%!  octavelibczi ('AddSubBlock', writer, 'Z1C0', [0 0 2 2], 'gray8', uint8(ones(2, 2) * 3), struct('M', 0));
%!  octavelibczi ('AddSubBlock', writer, 'Z1C1', [0 0 2 2], 'gray8', uint8(ones(2, 2) * 4), struct('M', 0));
%!  octavelibczi ('AddSubBlock', writer, 'C0T5', [10 10 2 2], 'gray8', uint8(ones(2, 2) * 5), struct('M', 1));
%!  octavelibczi ('CloseCziWriter', writer);
%!  handle = octavelibczi ('Open', filename);
%!endfunction

%!test
%! handle = create_test_file('test_qsbd_all.czi');
%! d = octavelibczi ('QuerySubBlockDirectory', handle);
%! assert(isa(d.Index, 'int32'));
%! assert(isa(d.Mode, 'uint8'));
%! assert(isa(d.Pixeltype, 'uint8'));
%! assert(isa(d.Coordinate, 'int32'));
%! assert(d.CoordinateDimensions, 'ZCT');
%! assert(size(d.Index), [5 1]);
%! assert(size(d.Coordinate), [5 3]);
%! assert(size(d.LogicalRect), [5 4]);
%! assert(size(d.PhysicalSize), [5 2]);
%! assert(sort(d.Index), int32((0:4)'));
%! % the order is unspecified, so look up the subblock with T=5 by its coordinate
%! row = find(d.Coordinate(:, 3) == 5);
%! assert(numel(row), 1);
%! assert(d.Coordinate(row, :), int32([intmin('int32') 0 5]));
%! assert(d.LogicalRect(row, :), int32([10 10 2 2]));
%! assert(d.MIndex(row), int32(1));
%! octavelibczi ('Close', handle);
%! delete('test_qsbd_all.czi');

%!test
%! handle = create_test_file('test_qsbd_coord.czi');
%! d = octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', 1)));
%! assert(d.CoordinateDimensions, 'ZC');
%! assert(sortrows(double(d.Coordinate)), [0 1; 1 1]);
%! d = octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', struct('Z', [0 2], 'C', 0)));
%! assert(sortrows(double(d.Coordinate)), [0 0; 1 0]);
%! d = octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', struct('Z', [1 5], 'C', [0 1])));
%! assert(double(d.Coordinate), [1 0]);
%! d = octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', struct('Z', 1, 'C', 1)));
%! assert(size(d.Index), [1 1]);
%! assert(octavelibczi ('GetSubBlockBitmap', handle, d.Index(1)), uint8([4 4; 4 4]));
%! octavelibczi ('Close', handle);
%! delete('test_qsbd_coord.czi');

%!test
%! handle = create_test_file('test_qsbd_roi.czi');
%! d = octavelibczi ('QuerySubBlockDirectory', handle, struct('Roi', [9 9 5 5]));
%! assert(size(d.Index), [1 1]);
%! assert(d.LogicalRect, int32([10 10 2 2]));
%! d = octavelibczi ('QuerySubBlockDirectory', handle, struct('OnlyLayer0', true));
%! assert(size(d.Index), [5 1]);
%! d = octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', 42)));
%! assert(isempty(d.Index));
%! assert(size(d.Coordinate, 1), 0);
%! octavelibczi ('Close', handle);
%! delete('test_qsbd_roi.czi');

%!test
%! handle = create_test_file('test_qsbd_args.czi');
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, 42)");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('Roi', [1 2]))");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', 1))");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', sparse([0 1]))))");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('Roi', sparse([9 9 5 5])))");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', 0.6)))");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', single(0.5))))");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('Roi', [9.5 9 5 5]))");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', [0 0])))");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', [0 1 2])))");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', single(2147483648))))");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('Roi', single([2147483648 0 1 1])))");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('OnlyLayer0', [1 0]))");
%! fail("octavelibczi ('QuerySubBlockDirectory', handle, struct('OnlyLayer0', sparse(1)))");
%! fail("octavelibczi ('QuerySubBlockDirectory', sparse(handle))");
%! octavelibczi ('Close', handle);
%! delete('test_qsbd_args.czi');

%!test
%! handle = create_test_file('test_qsbd_uint32.czi');
%! d = octavelibczi ('QuerySubBlockDirectory', handle, struct('Coordinate', struct('Z', uint32(0), 'C', uint32([0 2]))));
%! assert(sortrows(double(d.Coordinate)), [0 0; 0 1]);
%! d = octavelibczi ('QuerySubBlockDirectory', handle, struct('Roi', uint32([9 9 5 5])));
%! assert(d.LogicalRect, int32([10 10 2 2]));
%! octavelibczi ('Close', handle);
%! delete('test_qsbd_uint32.czi');
