file(GLOB_RECURSE LIBS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
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
)

target_link_libraries(libs PRIVATE
    Qt6::Widgets
)
