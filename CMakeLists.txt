# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.11)

set(MEXCZI_MAJOR 0)
set(MEXCZI_MINOR 1)
set(MEXCZI_PATCH 0)
set(MEXCZI_EXT "alpha")

#configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/Mathematica/CZIReader/PacletInfo.wl.in"  "${CMAKE_CURRENT_SOURCE_DIR}/Mathematica/CZIReader/PacletInfo.wl")

if(WIN32)
  # use "static C-runtime" -> https://stackoverflow.com/questions/14172856/compile-with-mt-instead-of-md-using-cmake
  # Note: this requires CMAKE version 3.15+
  cmake_policy(SET CMP0091 NEW)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif(WIN32)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})

project ("MEXCZI")

# include RapidJSON -> https://www.jibbow.com/posts/rapidjson-cmake/
#include("${CMAKE_SOURCE_DIR}/modules/rapidJSON.cmake")

#message(STATUS ${CMAKE_MODULE_PATH} )

#[[ 
if(WLLCZI_MATHEMATICAHEADERS)
  set(Mathematica_INCLUDE_DIR ${WLLCZI_MATHEMATICAHEADERS})
else()
  find_package(Mathematica)
endif(WLLCZI_MATHEMATICAHEADERS)

message(STATUS "Mathematica_INCLUDE_DIR: ${Mathematica_INCLUDE_DIR}")
message(STATUS "Mathematica_WolframLibrary_LIBRARIES: ${Mathematica_WolframLibrary_LIBRARIES}")
]]

include("${CMAKE_SOURCE_DIR}/modules/libCZI.cmake")
FetchContent_GetProperties(libCZI)
set(LIBCZI_INCLUDE_DIR "${libczi_SOURCE_DIR}/Src/libCZI")
message(STATUS ${LIBCZI_INCLUDE_DIR})

find_package(Matlab REQUIRED)

message("########################################")
message(STATUS "Matlab_ROOT_DIR: " ${Matlab_ROOT_DIR})
message(STATUS "Matlab_INCLUDE_DIR: " ${Matlab_INCLUDE_DIRS})
message(STATUS "Matlab_MEX_LIBRARY: " ${Matlab_MEX_LIBRARY})
message(STATUS "Matlab_MX_LIBRARY: " ${Matlab_MX_LIBRARY})
message(STATUS "Matlab_LIBRARIES: " ${Matlab_LIBRARIES}) 
message("########################################")

add_subdirectory ("MEXlibCZI")
