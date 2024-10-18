set(libmexlibcziSourceFiles
            "exportedfunctions.cpp"
)

add_library (libmexlibczi SHARED
			       ${libmexlibcziSourceFiles})

target_compile_features(libmexlibczi PRIVATE c_std_11)
set_target_properties(libmexlibczi PROPERTIES CXX_STANDARD 17)
target_link_libraries(libmexlibczi PRIVATE lib)
target_include_directories(libmexlibczi PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../lib" "${CMAKE_CURRENT_SOURCE_DIR}/../AppModel/include")

if(UNIX)
 set_target_properties(libmexlibczi PROPERTIES PREFIX "") # otherwise "lib" is prepended
endif()