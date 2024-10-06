set (libSourceFiles
			include/function.h
			src/function.cpp
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
 "src/implementation/CziWriter.cpp" "src/implementation/CziWriter.h")

if(CMAKE_BUILD_TYPE MATCHES Debug)
 set(lib_ENABLE_LOGGING 1)
else()
 set(lib_ENABLE_LOGGING 0)
endif()

set(lib_LOGLEVEL 1)


set(lib_VERSION_MAJOR	${MEXCZI_MAJOR})
set(lib_VERSION_MINOR	${MEXCZI_MINOR})
set(lib_VERSION_PATCH	${MEXCZI_PATCH})
set(lib_VERSION_EXT    ${MEXCZI_EXT})

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