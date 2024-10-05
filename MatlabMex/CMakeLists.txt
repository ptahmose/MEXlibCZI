
set(matlabMexSourceFiles
  					"app_api_implementation.c"
            "app_api_implementation.h"
            "mex_function.cpp"
)

add_library (MatlabCZI SHARED
			       ${matlabMexSourceFiles})
#             $<TARGET_OBJECTS:lib>)

target_compile_features(MatlabCZI PRIVATE c_std_11)
target_link_libraries(MatlabCZI PRIVATE ${Matlab_MEX_LIBRARY} ${Matlab_MX_LIBRARY} lib)
target_include_directories(MatlabCZI PRIVATE ${Matlab_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/../AppModel/include")
target_include_directories(MatlabCZI PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

target_compile_definitions(MatlabCZI PRIVATE MW_NEEDS_VERSION_H=1)

set_target_properties(MatlabCZI PROPERTIES SUFFIX ".mexw64")