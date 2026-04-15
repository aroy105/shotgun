cmake_minimum_required(VERSION 3.23)
project(shotgun LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(shotgun
    src/main.cpp
    src/kuhn.cpp
)

target_include_directories(shotgun PRIVATE include)