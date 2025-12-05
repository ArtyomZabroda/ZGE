cmake_minimum_required(VERSION 3.23)

project(ZGE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

enable_testing()

add_subdirectory("core")
add_subdirectory("render")
add_subdirectory("scene")
add_subdirectory("main")
add_subdirectory("sandbox")