% File: test_basic_operation.m
classdef TestMEXlibCZI_Basic_Operation < matlab.unittest.TestCase

    methods (Test)
        function testGetVersion_VersionString(testCase)
            version = MEXlibCZI('GetVersion');
            testCase.verifyTrue(checkVersionString(version.VersionString));
        end

        function testGetVersion_libCZIVersion(testCase)
            version = MEXlibCZI('GetVersion');
            testCase.verifyTrue(checkVersionString(version.libCZIVersion));
        end

        function testGetVersion_CompilerIdentification(testCase)
            version = MEXlibCZI('GetVersion');
            testCase.verifyTrue(~isempty(version.CompilerIdentification));
        end
    end
end

% Local function outside of the class definition
function result = checkVersionString(versionString)
    % Check if the VersionString starts with '0.3.0'
    match = regexp(versionString, '^\d{1,3}\.\d{1,3}\.\d{1,3}', 'once');
    if isempty(match)
        result = false; % No match found, return false
    else
        result = true;  % Match found, return true
    end
end