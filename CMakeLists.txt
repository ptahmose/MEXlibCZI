# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.11)

set(MEXCZI_MAJOR 0)
set(MEXCZI_MINOR 2)
set(MEXCZI_PATCH 1)
set(MEXCZI_EXT "alpha")

if(WIN32)
  # use "static C-runtime" -> https://stackoverflow.com/questions/14172856/compile-with-mt-instead-of-md-using-cmake
  # Note: this requires CMAKE version 3.15+
  cmake_policy(SET CMP0091 NEW)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif(WIN32)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})

project ("MEXlibCZI")

option(MEXLIBCZI_BUILD_MATLABMEX "Build mex-file for MATLAB" ON)
option(MEXLIBCZI_BUILD_OCTAVEOCT "Build oct-file for Octave" OFF)


# include RapidJSON -> https://www.jibbow.com/posts/rapidjson-cmake/
include("${CMAKE_SOURCE_DIR}/modules/rapidJSON.cmake")


include("${CMAKE_SOURCE_DIR}/modules/libCZI.cmake")
FetchContent_GetProperties(libCZI)
set(LIBCZI_INCLUDE_DIR "${libczi_SOURCE_DIR}/Src/libCZI")
message(STATUS ${LIBCZI_INCLUDE_DIR})

if(MEXLIBCZI_BUILD_MATLABMEX)
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

if(MEXLIBCZI_BUILD_OCTAVEOCT)
  set(OCTAVE_ROOT_DIR "D:/SW/octave-9.2.0-w64")
  set(OCTAVE_CONFIGEXECUTABLE_PATHHINT "D:/SW/octave-9.2.0-w64/mingw64/bin/")
  find_package(Octave REQUIRED)
  message("########################################")
  message(STATUS "Octave_INCLUDE_DIRS: " ${OCTAVE_INCLUDE_DIRS})
  message(STATUS "Octave_LIBRARIES: " ${OCTAVE_LIBRARIES})
  message("########################################")
endif()




#[[ 
set(OCTAVE_CONFIGEXECUTABLE_PATHHINT "D:/SW/Octave/Octave-5.2.0/mingw64/bin/")
find_package(Octave)
message("########################################")
message(STATUS "OCTAVE_INCLUDE_DIRS: " ${OCTAVE_INCLUDE_DIRS})
message("########################################")
]]

add_subdirectory ("MEXlibCZI")
