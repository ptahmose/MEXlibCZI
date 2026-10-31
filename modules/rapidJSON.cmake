# A source bundle supplies the exact reviewed headers without any network access.
set(MEXLIBCZI_RAPIDJSON_SOURCE_DIR "" CACHE PATH "Existing RapidJSON source directory")
if(MEXLIBCZI_RAPIDJSON_SOURCE_DIR)
  if(NOT EXISTS "${MEXLIBCZI_RAPIDJSON_SOURCE_DIR}/include/rapidjson/rapidjson.h")
    message(FATAL_ERROR "MEXLIBCZI_RAPIDJSON_SOURCE_DIR does not contain RapidJSON headers")
  endif()
  add_custom_target(rapidjson)
  set(RAPIDJSON_INCLUDE_DIR "${MEXLIBCZI_RAPIDJSON_SOURCE_DIR}/include")
  return()
endif()

include(ExternalProject)

# Download RapidJSON
ExternalProject_Add(
    rapidjson
    PREFIX "vendor/rapidjson"
    GIT_REPOSITORY "https://github.com/Tencent/rapidjson.git"
    GIT_TAG 815e6e7e7e14be44a6c15d9aefed232ff064cad0
    TIMEOUT 10
    CMAKE_ARGS
        -DRAPIDJSON_BUILD_TESTS=OFF
        -DRAPIDJSON_BUILD_DOC=OFF
        -DRAPIDJSON_BUILD_EXAMPLES=OFF
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)

# Prepare RapidJSON (RapidJSON is a header-only library)
ExternalProject_Get_Property(rapidjson source_dir)
set(RAPIDJSON_INCLUDE_DIR ${source_dir}/include)
