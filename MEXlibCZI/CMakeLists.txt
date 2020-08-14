if(CMAKE_BUILD_TYPE MATCHES Debug)
 set(mexlibczi_ENABLE_LOGGING 1)
else()
 set(mexlibczi_ENABLE_LOGGING 0)
endif()

set(mexlibczi_LOGLEVEL 1)

set(mexlibczi_VERSION_MAJOR	${MEXCZI_MAJOR})
set(mexlibczi_VERSION_MINOR	${MEXCZI_MINOR})
set(mexlibczi_VERSION_PATCH	${MEXCZI_PATCH})
set(mexlibczi_VERSION_EXT    ${MEXCZI_EXT})

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/mexlibczi_Config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/mexlibczi_Config.h"
	ESCAPE_QUOTES @ONLY)

add_library (MEXlibCZI SHARED
						"mexFunction.cpp"
						"mexFunctions.h" 
						"include_matlabheaders.h" 
						"mexFunctions.cpp" 
						"src/libraryInfo.h" 
						"src/libraryInfo.cpp" 
						"src/inc_libczi.h"
						"src/func_getversion.h" 
						"src/func_getversion.cpp" 
						"src/func_open.cpp" 
						"src/func_getsubblockbitmap.h" 
						"src/func_getsubblockbitmap.cpp"
						"src/CziInstanceManager.h" 
						"src/CziInstanceManager.cpp" 
						"src/CziReader.h" 
						"src/CziReader.cpp"
						"src/argsutils.h"
						"src/argutils.cpp" 
						"src/func_getmultichannelscalingtilecomposite.h" 
						"src/func_getmultichannelscalingtilecomposite.cpp"
						"src/CziUtilities.h" 
						"src/CziUtilities.cpp"
						"src/utils.h"
						"src/utils.cpp" 
						"src/func_getinfo.h" 
						"src/func_getinfo.cpp"
						"src/func_getscaling.h" 
						"src/func_getscaling.cpp" 
						"src/func_getmetadataxml.cpp"
						"src/func_getmetadataxml.h" 
						"src/func_getsinglechannelscalingtilecomposite.cpp" 
						"src/func_getsinglechannelscalingtilecomposite.h")

set_target_properties(MEXlibCZI PROPERTIES CXX_STANDARD 11)

target_include_directories(MEXlibCZI PRIVATE ${Matlab_INCLUDE_DIRS} ${RAPIDJSON_INCLUDE_DIR})

target_include_directories(MEXlibCZI PRIVATE  ${CMAKE_CURRENT_BINARY_DIR})  # this is necessary so that we can find "wllczi_Config.h" which we created above

#target_link_directories(MEXlibCZI PRIVATE mx mex mat)
#target_link_libraries(MEXlibCZI PRIVATE mx mex mat)
target_link_libraries(MEXlibCZI PRIVATE ${Matlab_MEX_LIBRARY} ${Matlab_MX_LIBRARY})

#set (CMAKE_LINK_DEF_FILE_FLAG "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/MEXlibCZI.def")

# target_compile_definitions(MEXlibCZI PRIVATE MEXlibCZI_EXPORTS MATLAB_MEX_FILE)
target_include_directories(MEXlibCZI PRIVATE ${LIBCZI_INCLUDE_DIR})
target_link_libraries(MEXlibCZI PRIVATE libCZIStatic JxrDecodeStatic)

set_target_properties(MEXlibCZI PROPERTIES SUFFIX ".mexw64")

SET_TARGET_PROPERTIES(MEXlibCZI PROPERTIES LINK_FLAGS "/export:mexFunction")