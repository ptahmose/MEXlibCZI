These tests exercise the shared argument helpers with a mock host API. Invalid
storage makes data access fail, so the tests also check that rejection happens
before reading array data. MATLAB and Octave are not required.

Enable with `-DMEXLIBCZI_BUILD_HELPER_TESTS=ON` when configuring the project,
then build and run `ctest --test-dir <build-directory> --output-on-failure`.
For a multi-configuration generator, select the build configuration with
`--config Release` for the build and `-C Release` for CTest.
