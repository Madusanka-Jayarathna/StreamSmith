# Create the library component
add_library(libs
    libMain/private/MainApp.cpp
    libMainView/private/MainView.cpp
    libMainView/public/MainView.h
)

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