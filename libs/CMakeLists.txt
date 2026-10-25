# Copyright (c) 2026 Madusanka Jayarathna. All rights reserved.
# Use of this source code is governed by the MIT license.

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

# FFmpeg detection
find_path(FFMPEG_INCLUDE_DIR
    NAMES libavformat/avformat.h
    HINTS
        /opt/homebrew/include
        /usr/local/include
        /usr/include
)

find_library(AVFORMAT_LIBRARY
    NAMES avformat
    HINTS
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    REQUIRED
)
find_library(AVCODEC_LIBRARY
    NAMES avcodec
    HINTS
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    REQUIRED
)
find_library(AVUTIL_LIBRARY
    NAMES avutil
    HINTS
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    REQUIRED
)


file(GLOB_RECURSE LIBS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(libs ${LIBS_SOURCES})

target_include_directories(libs PUBLIC
    libMain/public
)

target_include_directories(libs PRIVATE
    .
    ${FFMPEG_INCLUDE_DIR}
)

target_link_libraries(libs PRIVATE
    nlohmann_json::nlohmann_json
    ${AVFORMAT_LIBRARY}
    ${AVCODEC_LIBRARY}
    ${AVUTIL_LIBRARY}
)

