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
)


add_library (lib 
			       ${libSourceFiles})

set_target_properties(lib PROPERTIES CXX_STANDARD 17)
target_include_directories(lib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../AppModel/include")
target_include_directories(lib PRIVATE ${LIBCZI_INCLUDE_DIR})
target_link_libraries(lib PRIVATE libCZIStatic JxrDecodeStatic)