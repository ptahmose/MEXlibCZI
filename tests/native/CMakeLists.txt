add_executable(argument_helpers_test argument_helpers_test.cpp)
target_compile_features(argument_helpers_test PRIVATE cxx_std_17)
target_include_directories(argument_helpers_test PRIVATE
  "${PROJECT_SOURCE_DIR}/AppModel/include"
  "${PROJECT_SOURCE_DIR}/lib/src/implementation"
  "${LIBCZI_INCLUDE_DIR}")
target_link_libraries(argument_helpers_test PRIVATE lib libCZIStatic)
add_test(NAME argument_helpers COMMAND argument_helpers_test)

add_executable(utf8_conversion_test utf8_conversion_test.cpp)
target_compile_features(utf8_conversion_test PRIVATE cxx_std_17)
target_include_directories(utf8_conversion_test PRIVATE
  "${PROJECT_SOURCE_DIR}/AppModel/include"
  "${PROJECT_SOURCE_DIR}/lib/src/implementation"
  "${LIBCZI_INCLUDE_DIR}")
target_link_libraries(utf8_conversion_test PRIVATE lib libCZIStatic)
add_test(NAME utf8_conversion COMMAND utf8_conversion_test)
