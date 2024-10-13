set(octaveOctSourceFiles
  					"octlibczi.cpp"
)

add_library (OctaveOct SHARED
			       ${octaveOctSourceFiles} "octavelibczi.c")

target_compile_features(OctaveOct PRIVATE c_std_11)
set_target_properties(OctaveOct PROPERTIES CXX_STANDARD 17)
target_include_directories(OctaveOct PRIVATE ${OCTAVE_INCLUDE_DIR}/../)
target_link_libraries(OctaveOct PRIVATE lib)
