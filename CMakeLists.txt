# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.11)

set(MEXLIBCZI_MAJOR 0)
set(MEXLIBCZI_MINOR 3)
set(MEXLIBCZI_PATCH 5)
set(MEXLIBCZI_EXT "alpha")

if(WIN32)
  # use "static C-runtime" -> https://stackoverflow.com/questions/14172856/compile-with-mt-instead-of-md-using-cmake
  # Note: this requires CMAKE version 3.15+
  cmake_policy(SET CMP0091 NEW)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif(WIN32)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})

project ("MEXlibCZI"
          VERSION ${MEXLIBCZI_MAJOR}.${MEXLIBCZI_MINOR}.${MEXLIBCZI_PATCH}
          DESCRIPTION "mex extension (for MATLAB and Octave) for reading/writing CZI-files with libCZI")

# Options controlling the CMake-build are gathered here

# When this option is ON, we build the mex extension for MATLAB. Otherwise, we don't build the mex extension for MATLAB.
option(MEXLIBCZI_BUILD_MATLABMEX "Build the mex extension for MATLAB" ON)

# include RapidJSON -> https://www.jibbow.com/posts/rapidjson-cmake/
include("${CMAKE_SOURCE_DIR}/modules/rapidJSON.cmake")

include("${CMAKE_SOURCE_DIR}/modules/libCZI.cmake")
FetchContent_GetProperties(libCZI)
set(LIBCZI_INCLUDE_DIR "${libczi_SOURCE_DIR}/Src/libCZI")
message(STATUS ${LIBCZI_INCLUDE_DIR})

if (MEXLIBCZI_BUILD_MATLABMEX)
  if(MEXLIBCZI_HEADERS)
   set(Matlab_INCLUDE_DIRS ${MEXLIBCZI_HEADERS})
   if(UNIX)
    set(Matlab_MEX_LIBRARY ${MEXLIBCZI_LIBS}/libmex.so)
    set(Matlab_MX_LIBRARY ${MEXLIBCZI_LIBS}/libmx.so)
   else()
    set(Matlab_MEX_LIBRARY ${MEXLIBCZI_LIBS}/libmex.lib)
    set(Matlab_MX_LIBRARY ${MEXLIBCZI_LIBS}/libmx.lib)
   endif()
  else()
   find_package(Matlab REQUIRED)
  endif()

  message("########################################")
  message(STATUS "Matlab_ROOT_DIR: " ${Matlab_ROOT_DIR})
  message(STATUS "Matlab_INCLUDE_DIR: " ${Matlab_INCLUDE_DIRS})
  message(STATUS "Matlab_MEX_LIBRARY: " ${Matlab_MEX_LIBRARY})
  message(STATUS "Matlab_MX_LIBRARY: " ${Matlab_MX_LIBRARY})
  message(STATUS "Matlab_LIBRARIES: " ${Matlab_LIBRARIES}) 
  message("########################################")
endif()

# build the static library containing the MEX functions
add_subdirectory("lib")

if (MEXLIBCZI_BUILD_MATLABMEX)
  # build the Matlab-MEX file
  add_subdirectory ("MEXlibCZI")
endif()

# build a dynamic library containing the MEX functions (which is used by the mex-file for Octave)
add_subdirectory("libmexlibczi")