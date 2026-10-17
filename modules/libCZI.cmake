include(FetchContent)

# Set necessary build options for libCZI
set(LIBCZI_BUILD_CZICMD OFF CACHE BOOL "" FORCE)
set(LIBCZI_BUILD_DYNLIB OFF CACHE BOOL "" FORCE)
set(LIBCZI_BUILD_UNITTESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  libCZI
  GIT_REPOSITORY https://github.com/ptahmose/libczi-zeiss.git
  #GIT_REPOSITORY https://github.com/ZEISS/libczi
  #GIT_TAG        jbl/XT
  GIT_TAG       jbl/XT_XZ-subblock-workaround_and_compatibility
)

# Fetch the content and make it available
FetchContent_MakeAvailable(libCZI)
