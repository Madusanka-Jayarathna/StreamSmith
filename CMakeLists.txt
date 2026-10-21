# Copyright (c) 2026 Madusanka Jayarathna. All rights reserved.
# Use of this source code is governed by the MIT license.

cmake_minimum_required(VERSION 3.16)

project(StreamSmith)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Boost CONFIG REQUIRED COMPONENTS filesystem system)
#set(CMAKE_AUTOMOC ON)

#set(EXTRA_COMPONENT_DIRS "libs")

add_subdirectory(libs)
add_subdirectory(main)
#add_subdirectory(tests)
#add_executable(MediaDownloader main.cpp)
#target_link_libraries(MediaDownloader PRIVATE Qt6::Widgets)