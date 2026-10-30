function run_argument_validation_tests(api)
% Shared host-level regression cases, executed by both MATLAB and Octave.
% Every expected rejection must use the project's catchable error identifier.
filename = [tempname() '.czi'];
resources = containers.Map('KeyType', 'char', 'ValueType', 'any');
cleanup = onCleanup(@() cleanup_resources(api, filename, resources)); %#ok<NASGU>
writer = api('CreateCziWriter', filename); % Optional overwrite argument omitted.
resources('CloseCziWriter') = writer;
pixels = uint8([1 2; 3 4]);
bad_integers = {[], [0 1], true, 'bad', struct(), sparse(1), ...
    complex(1, 1), complex(single(1), single(1)), 0.5, -1.7, NaN, Inf, -Inf, ...
    single(NaN), single(Inf), single(2147483648), 2147483648, -2147483649, ...
    uint32(2147483648), int64(2147483648), int64(-2147483649), intmax('uint64'), sparse(0)};
% Some hosts preserve complex storage even when the imaginary part is zero.
zero_imaginary = complex(1, 0);
if ~isreal(zero_imaginary)
    bad_integers{end + 1} = zero_imaginary;
end

% Writer handles, numeric pixel identifiers, and optional M values.
fprintf('Argument validation: writer inputs\n');
for k = 1:numel(bad_integers)
    bad = bad_integers{k};
    expect_invalid(api, 0, {'CloseCziWriter', bad});
    expect_invalid(api, 1, {'Writer_GetPreparedMetadata', bad});
    expect_invalid(api, 0, {'Writer_WriteMetadata', bad, '<ImageDocument><Metadata/></ImageDocument>'});
    expect_invalid(api, 0, {'AddSubBlock', bad, 'C0', [0 0 2 2], 'gray8', pixels});
    expect_invalid(api, 0, {'AddSubBlock', writer, 'C0', [0 0 2 2], bad, pixels});
    expect_invalid(api, 0, {'AddSubBlock', writer, 'C0', [0 0 2 2], 'gray8', pixels, struct('M', bad)});
end
bad_rois = {[], [0 0 2], [0 0 0.5 2], [0 0 -1 2], [0 0 2 -1], ...
    [NaN 0 2 2], [0 Inf 2 2], [0 0 2147483648 2], ...
    single([0 0 2147483648 2]), complex([0 0 2 2], [1 0 0 0]), ...
    sparse([0 0 2 2]), true(1, 4), 'bad', struct()};
for k = 1:numel(bad_rois)
    expect_invalid(api, 0, {'AddSubBlock', writer, 'C0', bad_rois{k}, 'gray8', pixels});
end
for bad = {complex(ones(2), ones(2)), sparse(ones(2)), true(2), uint8([]), ...
        zeros(2), uint16(ones(2)), uint8(ones(2, 2, 3)), uint8(ones(2, 2, 2)), ...
        uint8(ones(2, 2, 2, 2))}
    % Includes mismatches between the selected pixel type and class/shape.
    expect_invalid(api, 0, {'AddSubBlock', writer, 'C0', [0 0 2 2], 'gray8', bad{1}});
end
expect_invalid(api, 0, {'AddSubBlock', writer, 'C0', [0 0 2 2], 'bgr24', pixels});
expect_invalid(api, 0, {'AddSubBlock', writer, 'C0', [0 0 2 2], 'gray16', pixels});
expect_invalid(api, 0, {'AddSubBlock', writer, 'C0', [0 0 2 2], 'gray8', pixels, struct([])});
expect_invalid(api, 0, {'AddSubBlock', writer, 'C0', [0 0 2 2], 'gray8', pixels, repmat(struct('M', 0), 1, 2)});

% All failed writes must leave the writer usable and add no subblocks.
% Legacy ROI arrays may have extra elements; only the first four are read.
api('AddSubBlock', writer, 'C0', [0 0 2 2 99.5], 'gray8', pixels, struct('M', uint32(0)));
builder = api('Writer_GetPreparedMetadata', writer);
resources('MetadataBuilder_Release') = builder;
original = api('MetadataBuilder_GetXml', builder);

% A valid X followed by invalid Y must not partially change the XML.
fprintf('Argument validation: metadata inputs\n');
bad_scales = {[], [1 2], true, 'bad', struct(), sparse(1e-6), ...
    complex(1e-6, 1e-6), complex(single(1e-6), single(1e-6)), ...
    NaN, Inf, -Inf, single(NaN), single(Inf), 0, -1, int32(1)};
for k = 1:numel(bad_scales)
    expect_invalid(api, 0, {'MetadataBuilder_Modify', builder, 'Scaling', ...
        struct('X', 2e-6, 'Y', bad_scales{k})});
    assert(isequal(api('MetadataBuilder_GetXml', builder), original), 'Rejected scaling changed XML');
end
for bad = {[], [1 2], true, sparse(1), complex(1, 1), NaN, Inf, 1.5, ...
        int32(-1), int32(6), uint32(1), 1}
    expect_invalid(api, 0, {'MetadataBuilder_Modify', builder, 'GeneralDocumentInfo', ...
        struct('Name', 'must not change', 'Rating', bad{1})});
    assert(isequal(api('MetadataBuilder_GetXml', builder), original), 'Rejected rating changed XML');
end
for k = 1:numel(bad_integers)
    bad = bad_integers{k};
    expect_invalid(api, 1, {'MetadataBuilder_GetXml', bad});
    expect_invalid(api, 0, {'MetadataBuilder_SetXml', bad, original});
    expect_invalid(api, 0, {'MetadataBuilder_Modify', bad, 'Scaling', struct('X', 1e-6)});
    expect_invalid(api, 0, {'MetadataBuilder_Release', bad});
end
api('MetadataBuilder_Modify', builder, 'Scaling', struct('X', 0.25e-6, 'Y', single(0.5e-6)));
api('MetadataBuilder_Modify', builder, 'GeneralDocumentInfo', struct('Rating', int32(5)));
xml = api('MetadataBuilder_GetXml', builder);
api('Writer_WriteMetadata', writer, xml);
api('CloseCziWriter', writer);

reader = api('Open', filename);
resources('Close') = reader;
directory = api('QuerySubBlockDirectory', reader);
assert(numel(directory.Index) == 1, 'Rejected writes added subblocks');
assert(isequal(api('GetSubBlockBitmap', reader, 0), pixels), 'Writer did not recover');
for numeric_class = {'double', 'single', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64'}
    bitmap = api('GetSubBlockBitmap', cast(reader, numeric_class{1}), cast(0, numeric_class{1}));
    assert(isequal(bitmap, pixels), ['Valid numeric class rejected: ' numeric_class{1}]);
end
scaling = api('GetScaling', reader);
assert(abs(scaling.scaleX - 0.25e-6) < 1e-18, 'Valid scaling did not persist');
persisted = api('GetMetadataXml', reader);
assert(~isempty(strfind(persisted, '<Rating>5</Rating>')), 'Metadata did not persist'); %#ok<STREMP>

% Every reader command rejects malformed handles before using them.
fprintf('Argument validation: reader handles and subblock indices\n');
reader_calls = {{'GetInfo'}, {'GetScaling'}, {'GetMetadataXml'}, ...
    {'GetDefaultDisplaySettings'}, {'QuerySubBlockDirectory'}, ...
    {'GetSubBlockBitmap', 0}, {'GetSubBlock', 0}, ...
    {'GetSingleChannelScalingTileComposite', [0 0 2 2], 'C0', 1}, ...
    {'GetMultiChannelScalingTileComposite', [0 0 2 2], 'C0', 1}};
for k = 1:numel(bad_integers)
    bad = bad_integers{k};
    expect_invalid(api, 0, {'Close', bad});
    for j = 1:numel(reader_calls)
        args = reader_calls{j};
        expect_invalid(api, 1, [args(1) {bad} args(2:end)]);
    end
    expect_invalid(api, 1, {'GetSubBlock', reader, bad});
    expect_invalid(api, 1, {'GetSubBlockBitmap', reader, bad});
    assert(isequal(api('GetSubBlockBitmap', reader, 0), pixels), 'Rejected reader call changed state');
end
for bad = {-1, intmax('int32')}
    expect_invalid(api, 1, {'GetSubBlock', reader, bad{1}});
    expect_invalid(api, 1, {'GetSubBlockBitmap', reader, bad{1}});
end

subblock = api('GetSubBlock', reader, 0);
fprintf('Argument validation: subblock handles\n');
subblock_calls = {'GetInfoFromSubBlock', 'GetMetadataFromSubBlock', ...
    'GetBitmapFromSubBlock', 'ReleaseSubBlock'};
for k = 1:numel(bad_integers)
    for j = 1:numel(subblock_calls)
        expect_invalid(api, 1, {subblock_calls{j}, bad_integers{k}, subblock});
        expect_invalid(api, 1, {subblock_calls{j}, reader, bad_integers{k}});
    end
    assert(isequal(api('GetBitmapFromSubBlock', reader, subblock), pixels), 'Rejected release lost subblock');
end
assert(api('ReleaseSubBlock', reader, subblock) == 1, 'Subblock could not be released');

% Complex/sparse/fractional values must also be rejected inside filter structs.
fprintf('Argument validation: directory filters\n');
for k = 1:numel(bad_integers)
    bad = bad_integers{k};
    if isequal(bad, [0 1])
        continue; % Valid [start size] interval, although not a scalar handle.
    end
    expect_invalid(api, 1, {'QuerySubBlockDirectory', reader, struct('Coordinate', struct('C', bad))});
end
for bad = {[0 0], [0 -1], [0 0.5], [0 Inf], [0 2147483648], [0 1 2]}
    expect_invalid(api, 1, {'QuerySubBlockDirectory', reader, struct('Coordinate', struct('C', bad{1}))});
end
for k = 1:numel(bad_rois)
    expect_invalid(api, 1, {'QuerySubBlockDirectory', reader, struct('Roi', bad_rois{k})});
end
expect_invalid(api, 1, {'QuerySubBlockDirectory', reader, struct('Roi', [0 0 2 2 5])});
expect_invalid(api, 1, {'QuerySubBlockDirectory', reader, struct([])});
expect_invalid(api, 1, {'QuerySubBlockDirectory', reader, repmat(struct('OnlyLayer0', true), 1, 2)});
expect_invalid(api, 1, {'QuerySubBlockDirectory', reader, struct('Coordinate', struct([]))});
expect_invalid(api, 1, {'QuerySubBlockDirectory', reader, struct('Coordinate', repmat(struct('C', 0), 1, 2))});
for bad = {[], [0 1], 'bad', struct(), sparse(true), complex(1, 1), ...
        0.5, NaN, Inf, single(2147483648), intmax('uint64')}
    expect_invalid(api, 1, {'QuerySubBlockDirectory', reader, struct('OnlyLayer0', bad{1})});
end
for flag = {false, true, 0, 1, int32(-1)}
    d = api('QuerySubBlockDirectory', reader, struct('OnlyLayer0', flag{1}));
    assert(numel(d.Index) == 1, 'Valid flag rejected or interpreted incorrectly');
end
d = api('QuerySubBlockDirectory', reader, struct('Coordinate', struct('C', uint32([0 1])), 'Roi', int32([0 0 2 2])));
assert(numel(d.Index) == 1, 'Valid integer filter rejected');

composites = {'GetSingleChannelScalingTileComposite', 'GetMultiChannelScalingTileComposite'};
bad_zooms = {[], [1 1], true, 'bad', struct(), sparse(1), complex(1, 1), ...
    complex(single(1), single(1)), NaN, Inf, -Inf, single(NaN), single(Inf), ...
    0, -1, 1.5, 1 + eps, realmax(), realmin()};
for j = 1:numel(composites)
    command = composites{j};
    fprintf('Argument validation: %s\n', command);
    for k = 1:numel(bad_rois)
        expect_invalid(api, 1, {command, reader, bad_rois{k}, 'C0', 1});
    end
    for k = 1:numel(bad_zooms)
        expect_invalid(api, 1, {command, reader, [0 0 2 2], 'C0', bad_zooms{k}});
    end
    for zoom = {1, single(0.5)}
        if strcmp(command, 'GetSingleChannelScalingTileComposite')
            bitmap = api(command, reader, [0 0 2 2 99.5], 'C0', zoom{1});
            assert(~isempty(bitmap), 'Valid composite rejected');
        else
            % Prepared metadata has no display settings: fail catchably.
            expect_invalid(api, 1, {command, reader, [0 0 2 2 99.5], 'C0', zoom{1}});
        end
    end
end
expect_invalid(api, 1, {'GetDefaultDisplaySettings', reader});
assert(isequal(api('GetSubBlockBitmap', reader, 0), pixels), 'Reader did not recover');
api('Close', reader);

% Automatic metadata includes the display settings needed by multichannel composites.
writer = api('CreateCziWriter', filename, 'x');
resources('CloseCziWriter') = writer;
api('AddSubBlock', writer, 'C0', [0 0 2 2], 'gray8', pixels);
api('CloseCziWriter', writer);
reader = api('Open', filename);
resources('Close') = reader;
for zoom = {1, single(0.5)}
    bitmap = api('GetMultiChannelScalingTileComposite', reader, [0 0 2 2 99.5], 'C0', zoom{1});
    assert(~isempty(bitmap), 'Valid multichannel composite rejected');
end
api('Close', reader);
api('MetadataBuilder_Release', builder);
end

function expect_invalid(api, output_count, args)
try
    outputs = cell(1, output_count);
    if output_count == 0
        api(args{:});
    else
        [outputs{:}] = api(args{:});
    end
catch exception
    assert(strcmp(exception.identifier, 'MEXlibCZI:InvalidArguments'), ...
        ['Unexpected error from ' args{1} ': ' exception.identifier ' / ' exception.message]);
    return;
end
error('Test:ExpectedRejection', 'Expected InvalidArguments from %s', args{1});
end

function cleanup_resources(api, filename, resources)
% Close files before deleting them, including when a test fails halfway through.
for command = {'Close', 'CloseCziWriter', 'MetadataBuilder_Release'}
    if isKey(resources, command{1})
        try
            api(command{1}, resources(command{1}));
        catch
            % Explicit close/release may already have invalidated the handle.
        end
    end
end
if exist(filename, 'file')
    delete(filename);
end
end
