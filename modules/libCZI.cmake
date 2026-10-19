include(FetchContent)

# Set necessary build options for libCZI
set(LIBCZI_BUILD_CZICMD OFF CACHE BOOL "" FORCE)
set(LIBCZI_BUILD_DYNLIB OFF CACHE BOOL "" FORCE)
set(LIBCZI_BUILD_UNITTESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  libCZI
  GIT_REPOSITORY https://github.com/ZEISS/libczi
  GIT_TAG       cf8fcaf43001551345bbbc22bb92c0a67e956aba  # version 0.71.0
)

# Fetch the content and make it available
FetchContent_MakeAvailable(libCZI)
