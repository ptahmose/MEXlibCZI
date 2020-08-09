include(FetchContent)


FetchContent_Declare(
  libCZI
  GIT_REPOSITORY https://github.com/ptahmose/libCZI.git
  GIT_TAG        origin/wolframliblink  # use the latest in this branch
  # this version is "known-to-work"
  #GIT_TAG        9aa6b6e411de217a9f2aa91909107def843fe1ec
  #CMAKE_ARGS "-DLIBCZI_BUILD_CZICMD=OFF -DLIBCZI_BUILD_DYNLIB=OFF"
  # ^ or like this - sometimes it's better because FetchContent won't look
  # into remote to see if branch head was updated or not - good for stable
  # tags like 'vX.X' corresponding to releases
)

if(NOT libCZI_POPULATED)
  message(STATUS "Fetching libCZI")
  FetchContent_Populate(libCZI)

  set(LIBCZI_BUILD_CZICMD OFF CACHE BOOL "" FORCE)
  set(LIBCZI_BUILD_DYNLIB OFF CACHE BOOL "" FORCE)
  set(LIBCZI_BUILD_UNITTESTS OFF CACHE BOOL "" FORCE)

  add_subdirectory(${libczi_SOURCE_DIR} ${libczi_BINARY_DIR})
endif()

