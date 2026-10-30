%!test
%! common = fullfile(fileparts(which('test_argument_validation')), '..', 'common');
%! addpath(common);
%! cleanup = onCleanup(@() rmpath(common));
%! run_argument_validation_tests(@octavelibczi);
