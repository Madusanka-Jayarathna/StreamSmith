add_executable(run_streamsmith
    main.cpp
)

# Equivalent to 'REQUIRES' in ESP-IDF
# Links internal libraries and external dependencies to main.cpp
target_link_libraries(run_streamsmith PRIVATE
    libs
    Qt6::Widgets
)

target_include_directories(run_streamsmith PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/libs
)
