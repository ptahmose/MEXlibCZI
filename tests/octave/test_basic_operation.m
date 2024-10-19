% File: test_basic_operation.m

function isValid = checkVersionString(versionStr)
    % Function to check if the version string follows "XXX.XXX.XXX" pattern
    pattern = '^\d{1,3}\.\d{1,3}\.\d{1,3}';
    isValid = ~isempty(regexp(versionStr, pattern, 'once'));
end

addpath('../../OctaveMex');

%!test
%! version=octavelibczi('GetVersion');
%! assert(checkVersionString(version.VersionString))

%!test
%! version=octavelibczi('GetVersion');
%! assert(checkVersionString(version.libCZIVersion))

%!test
%! version=octavelibczi('GetVersion');
%! assert(~isempty(version.CompilerIdentification))

%!test
%! fail ("octavelibczi('Open','non_existing_file.czi')")