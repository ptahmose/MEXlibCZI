classdef TestMEXlib_ArgumentValidation < matlab.unittest.TestCase
    methods (Test)
        function rejectsInvalidInputsAndPreservesState(testCase)
            common = fullfile(fileparts(mfilename('fullpath')), '..', 'common');
            testCase.applyFixture(matlab.unittest.fixtures.PathFixture(common));
            run_argument_validation_tests(@MEXlibCZI);
        end
    end
end
