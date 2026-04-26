cmake_minimum_required(VERSION 3.23)
project(shotgun LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.4
)

FetchContent_MakeAvailable(Catch2)

add_executable(shotgun
    src/main.cpp
    src/kuhn.cpp
    src/cfr.cpp
)

target_include_directories(shotgun PRIVATE include)

add_executable(shotgun_tests
    tests/test_kuhn.cpp
    src/kuhn.cpp
    src/cfr.cpp
)

target_include_directories(shotgun_tests PRIVATE include)
target_link_libraries(shotgun_tests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)

catch_discover_tests(shotgun_tests)
