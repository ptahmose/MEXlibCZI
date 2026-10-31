
set(matlabMexSourceFiles
  					"app_api_implementation.c"
            "app_api_implementation.h"
            "mex_function.cpp"
)

add_library (MEXlibCZI SHARED
			       ${matlabMexSourceFiles})

target_compile_features(MEXlibCZI PRIVATE c_std_11)
set_target_properties(MEXlibCZI PROPERTIES CXX_STANDARD 17)
target_link_libraries(MEXlibCZI PRIVATE ${Matlab_MEX_LIBRARY} ${Matlab_MX_LIBRARY} lib)
target_include_directories(MEXlibCZI PRIVATE ${Matlab_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/../AppModel/include")
target_include_directories(MEXlibCZI PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

# this definition is necessary so that "mexFunction" is exported
target_compile_definitions(MEXlibCZI PRIVATE MW_NEEDS_VERSION_H=1)

if(WIN32)
 set_target_properties(MEXlibCZI PROPERTIES SUFFIX ".mexw64")
elseif(APPLE)
 # FindMatlab detects the extension for the installed MATLAB architecture.
 if(Matlab_MEX_EXTENSION)
  set_target_properties(MEXlibCZI PROPERTIES SUFFIX ".${Matlab_MEX_EXTENSION}")
 elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
  set_target_properties(MEXlibCZI PROPERTIES SUFFIX ".mexmaca64")
 else()
  set_target_properties(MEXlibCZI PROPERTIES SUFFIX ".mexmaci64")
 endif()
 set_target_properties(MEXlibCZI PROPERTIES PREFIX "")
elseif(UNIX)
 set_target_properties(MEXlibCZI PROPERTIES SUFFIX ".mexa64")
 target_link_options(MEXlibCZI PRIVATE -static-libgcc -static-libstdc++ )
 set_target_properties(MEXlibCZI PROPERTIES PREFIX "") # otherwise "lib" is prepended
endif()