
set(matlabMexSourceFiles
  					"app_api_implementation.c"
            "app_api_implementation.h"
            "mex_function.cpp"
)

add_library (MatlabCZI SHARED
			       ${matlabMexSourceFiles})
#             $<TARGET_OBJECTS:lib>)

target_compile_features(MatlabCZI PRIVATE c_std_11)
set_target_properties(MatlabCZI PROPERTIES CXX_STANDARD 17)
target_link_libraries(MatlabCZI PRIVATE ${Matlab_MEX_LIBRARY} ${Matlab_MX_LIBRARY} lib)
target_include_directories(MatlabCZI PRIVATE ${Matlab_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/../AppModel/include")
target_include_directories(MatlabCZI PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

# this definition is necessary so that "mexFunction" is exported
target_compile_definitions(MatlabCZI PRIVATE MW_NEEDS_VERSION_H=1)

if(WIN32)
 set_target_properties(MatlabCZI PROPERTIES SUFFIX ".mexw64")
elseif(UNIX)
 set_target_properties(MatlabCZI PROPERTIES SUFFIX ".mexa64")
 target_link_options(MatlabCZI PRIVATE -static-libgcc -static-libstdc++ )
 set_target_properties(MatlabCZI PROPERTIES PREFIX "") # otherwise "lib" is prepended
endif()