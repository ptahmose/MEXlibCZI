

add_library (MEXlibCZI SHARED
						mexFunction.cpp 
						"mexFunctions.h" 
						"include_matlabheaders.h" 
						"mexFunctions.cpp" 
						"src/libraryInfo.h" 
						"src/libraryInfo.cpp" 
						"src/inc_libczi.h")

set_target_properties(MEXlibCZI PROPERTIES CXX_STANDARD 11)

target_include_directories(MEXlibCZI PRIVATE ${Matlab_INCLUDE_DIRS})

#target_link_directories(MEXlibCZI PRIVATE mx mex mat)
#target_link_libraries(MEXlibCZI PRIVATE mx mex mat)
target_link_libraries(MEXlibCZI PRIVATE ${Matlab_MEX_LIBRARY} ${Matlab_MX_LIBRARY})

#set (CMAKE_LINK_DEF_FILE_FLAG "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/MEXlibCZI.def")

# target_compile_definitions(MEXlibCZI PRIVATE MEXlibCZI_EXPORTS MATLAB_MEX_FILE)
target_include_directories(MEXlibCZI PRIVATE ${LIBCZI_INCLUDE_DIR})
target_link_libraries(MEXlibCZI PRIVATE libCZIStatic JxrDecodeStatic)

set_target_properties(MEXlibCZI PROPERTIES SUFFIX ".mexw64")

SET_TARGET_PROPERTIES(MEXlibCZI PROPERTIES LINK_FLAGS "/export:mexFunction")