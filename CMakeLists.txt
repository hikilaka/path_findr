cmake_minimum_required(VERSION 3.8)

project(path_findr)

add_compile_options(-Wall -Wextra -Wpedantic -g)

add_executable(path_findr src/main.c src/ordered_set.c src/map.c src/path.c)

target_compile_features(path_findr PRIVATE c_std_99)

target_include_directories(path_findr PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include")
