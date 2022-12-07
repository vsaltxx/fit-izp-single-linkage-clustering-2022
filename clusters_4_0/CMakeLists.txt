cmake_minimum_required(VERSION 3.24)
project(clusters_4_0 C)

set(CMAKE_C_STANDARD 99)

add_executable(clusters_4_0 cluster.c)
target_link_libraries(clusters_4_0 m)