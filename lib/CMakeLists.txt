set (libSourceFiles
			"include/function.h"
			"src/function.cpp"
			"src/mexFunctions.h" 
			"src/errorcodes.h" 
			"src/errorcodes.cpp" 
			"src/mexFunctions.cpp" 
			"src/functions/func_getversion.h" 
			"src/functions/func_getversion.cpp"
			"src/implementation/inc_libczi.h"
			"src/implementation/libraryinfo.h"
			"src/implementation/libraryinfo.cpp"
			"src/implementation/CziReaderManager.h" 
		  "src/implementation/CziReaderManager.cpp"
			"src/implementation/CziReader.h"
			"src/implementation/CziReader.cpp"
			"src/implementation/dbgprint.h"
			"src/implementation/dbgprint.cpp"
			"src/implementation/CziReaderSbBlkStore.cpp" 
			"src/implementation/CziReaderSbBlkStore.h" 
			"src/implementation/CziUtilities.cpp" 
			"src/implementation/CziUtilities.h"
			"src/implementation/include_rapidjson.h"
			"src/implementation/argsutils.h"
			"src/implementation/argsutils.cpp"
			"src/implementation/CziWriter.cpp" 
			"src/implementation/CziWriter.h"
			"src/implementation/utils.cpp" 
			"src/implementation/utils.h"
			"src/implementation/CziWriterManager.cpp" 
			"src/implementation/CziWriterManager.h" 
			"src/functions/func_open.cpp" 
			"src/functions/func_open.h" 
			"src/functions/func_getsubblockbitmap.cpp" 
			"src/functions/func_getsubblockbitmap.h" 
			"src/functions/func_getmultichannelscalingtilecomposite.cpp" 
			"src/functions/func_getmultichannelscalingtilecomposite.h" 
			"src/functions/func_getinfo.cpp" 
			"src/functions/func_getinfo.h" 
			"src/functions/func_getmetadataxml.cpp" 
			"src/functions/func_getmetadataxml.h" 
			"src/functions/func_getscaling.cpp" 
			"src/functions/func_getscaling.h"
			"src/functions/func_getsinglechannelscalingtilecomposite.h"
			"src/functions/func_getsinglechannelscalingtilecomposite.cpp"
			"src/functions/func_close.cpp" 
			"src/functions/func_close.h"
			"src/functions/func_getdefaultdisplaysettings.cpp" 
			"src/functions/func_getdefaultdisplaysettings.h" 
			"src/functions/func_getsubblock.cpp" 
			"src/functions/func_getsubblock.h" 
			"src/functions/func_createcziwriter.cpp" 
			"src/functions/func_createcziwriter.h" 
			"src/functions/func_addsubblock.cpp" 
			"src/functions/func_addsubblock.h" 
			"src/functions/func_closecziwriter.cpp" 
			"src/functions/func_closecziwriter.h")

if(CMAKE_BUILD_TYPE MATCHES Debug)
 set(lib_ENABLE_LOGGING 1)
else()
 set(lib_ENABLE_LOGGING 0)
endif()

set(lib_LOGLEVEL 1)


set(lib_VERSION_MAJOR	${MEXLIBCZI_MAJOR})
set(lib_VERSION_MINOR	${MEXLIBCZI_MINOR})
set(lib_VERSION_PATCH	${MEXLIBCZI_PATCH})
set(lib_VERSION_EXT   ${MEXLIBCZI_EXT})

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/lib_config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/lib_config.h"
	ESCAPE_QUOTES @ONLY)


add_library (lib 
			       ${libSourceFiles})

set_target_properties(lib PROPERTIES CXX_STANDARD 17)
target_include_directories(lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../AppModel/include")
target_include_directories(lib PRIVATE ${LIBCZI_INCLUDE_DIR}  ${RAPIDJSON_INCLUDE_DIR})
target_link_libraries(lib PRIVATE libCZIStatic JxrDecodeStatic)
target_include_directories(lib PRIVATE  ${CMAKE_CURRENT_BINARY_DIR})  # this is necessary so that we can find "lib_config.h" which we created above

if(WIN32)
 target_compile_definitions(lib PRIVATE _WIN32API=1)
endif()
if(UNIX)
  # Enable PIC for the static library
	set_target_properties(lib PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()