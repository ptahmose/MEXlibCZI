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
						"src/func_getsinglechannelscalingtilecomposite.h"
						"src/func_close.cpp" 
						"src/func_close.h" 
						"src/mexapi.h" 
						"src/mexapi.cpp"
						"errorcodes.h"
						"errorcodes.cpp" 
						"src/dbgprint.h" 
						"src/dbgprint.cpp")

set_target_properties(MEXlibCZI PROPERTIES CXX_STANDARD 11)

  
target_include_directories(MEXlibCZI PRIVATE ${Matlab_INCLUDE_DIRS} ${RAPIDJSON_INCLUDE_DIR})

target_include_directories(MEXlibCZI PRIVATE  ${CMAKE_CURRENT_BINARY_DIR})  # this is necessary so that we can find "wllczi_Config.h" which we created above
target_include_directories(MEXlibCZI PRIVATE ${LIBCZI_INCLUDE_DIR})
target_link_libraries(MEXlibCZI PRIVATE ${Matlab_MEX_LIBRARY} ${Matlab_MX_LIBRARY})
target_link_libraries(MEXlibCZI PRIVATE libCZIStatic JxrDecodeStatic)

if(WIN32)
 set_target_properties(MEXlibCZI PROPERTIES SUFFIX ".mexw64")
elseif(UNIX)
 set_target_properties(MEXlibCZI PROPERTIES SUFFIX ".mexa64")
endif()

#SET_TARGET_PROPERTIES(MEXlibCZI PROPERTIES LINK_FLAGS "/export:mexFunction")
SET_TARGET_PROPERTIES (MEXlibCZI PROPERTIES DEFINE_SYMBOL "MATLABMEXBUILD")
#SET_TARGET_PROPERTIES (MEXlibCZI PROPERTIES DEFINE_SYMBOL "MW_NEEDS_VERSION_H")

# this definition is necessary so that "mexFunction" is exported
target_compile_definitions(MEXlibCZI PRIVATE MW_NEEDS_VERSION_H=1)

if(WIN32)
 target_compile_definitions(MEXlibCZI PRIVATE _WIN32API=1)
endif()


#[[  
# building a mex-file for Octave:
# * "mx"-functions are found in liboctinterp-7 - DLL
# * need to build an import-library for the DLL
# * not all mx-functions are present (not the typed "mxGetInt8s"-etc. functions, not mxArrayToUTF8String)
# * extension must be ".mex" I suppose, not ".mexw64" or so

target_include_directories(MEXlibCZI PRIVATE ${OCTAVE_INCLUDE_DIRS} ${RAPIDJSON_INCLUDE_DIR})

target_include_directories(MEXlibCZI PRIVATE  ${CMAKE_CURRENT_BINARY_DIR})  # this is necessary so that we can find "wllczi_Config.h" which we created above
target_include_directories(MEXlibCZI PRIVATE ${LIBCZI_INCLUDE_DIR})
target_link_libraries(MEXlibCZI PRIVATE "D:/Temp/octave/liboctinterp-7.lib")
target_link_libraries(MEXlibCZI PRIVATE libCZIStatic JxrDecodeStatic)

set_target_properties(MEXlibCZI PROPERTIES SUFFIX ".mex")
SET_TARGET_PROPERTIES(MEXlibCZI PROPERTIES LINK_FLAGS "/export:mexFunction")
SET_TARGET_PROPERTIES (MEXlibCZI PROPERTIES DEFINE_SYMBOL  "OCTAVEMEXBUILD" )
]]