include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

# FFmpeg (Homebrew) - no CMake config package or pkg-config available
find_library(AVFORMAT_LIBRARY avformat REQUIRED)
find_library(AVCODEC_LIBRARY  avcodec  REQUIRED)
find_library(AVUTIL_LIBRARY   avutil   REQUIRED)


file(GLOB_RECURSE LIBS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(libs ${LIBS_SOURCES})

# Equivalent to PRIV_INCLUDE_DIRS / INCLUDE_DIRS
# 'PUBLIC' means anyone linking against this library gets this include folder automatically
target_include_directories(libs PUBLIC
    libMain/public
)

target_include_directories(libs PRIVATE
    .
    /opt/homebrew/include
)

target_link_libraries(libs PRIVATE
    #Qt6::Widgets
    nlohmann_json::nlohmann_json
    Boost::filesystem
    Boost::system
    ${AVFORMAT_LIBRARY}
    ${AVCODEC_LIBRARY}
    ${AVUTIL_LIBRARY}
)

