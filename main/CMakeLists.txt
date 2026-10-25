# Copyright (c) 2026 Madusanka Jayarathna. All rights reserved.
# Use of this source code is governed by the MIT license.

add_executable(streamsmith
    main.cpp
)

target_link_libraries(streamsmith PRIVATE
    libs
)

target_include_directories(streamsmith PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/libs
)
