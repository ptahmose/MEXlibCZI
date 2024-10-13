set(libmexlibcziSourceFiles
  					"DllMain.cpp"
            "exportedfunctions.cpp"
)

add_library (libmexlibczi SHARED
			       ${libmexlibcziSourceFiles})
#             $<TARGET_OBJECTS:lib>)

target_compile_features(libmexlibczi PRIVATE c_std_11)
set_target_properties(libmexlibczi PROPERTIES CXX_STANDARD 17)
target_link_libraries(libmexlibczi PRIVATE lib)
#target_include_directories(libmexlibczi PRIVATE ${Matlab_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/../AppModel/include")
target_include_directories(libmexlibczi PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../lib" "${CMAKE_CURRENT_SOURCE_DIR}/../AppModel/include")

if(WIN32)
 #set_target_properties(MatlabCZI PROPERTIES SUFFIX ".mexw64")
elseif(UNIX)
 #set_target_properties(MatlabCZI PROPERTIES SUFFIX ".mexa64")
 target_link_options(libmexlibczi PRIVATE -static-libgcc -static-libstdc++ )
 set_target_properties(MatlabCZI PROPERTIES PREFIX "") # otherwise "lib" is prepended
endif()