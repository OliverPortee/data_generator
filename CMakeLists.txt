
cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(data_generator)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB INC_SOURCES "include/data_generator/*.hpp")
file(GLOB CLI_SOURCES "cli/*.cpp")

FetchContent_Declare(cli11 GIT_REPOSITORY https://github.com/CLIUtils/CLI11 GIT_TAG v2.2.0)
FetchContent_MakeAvailable(cli11)

FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.0.1)
FetchContent_MakeAvailable(Catch2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wconversion -Wsign-conversion -Wpedantic")

add_library(libdatagen ${INC_SOURCES})
target_include_directories(libdatagen PUBLIC "include")
set_target_properties(libdatagen PROPERTIES LINKER_LANGUAGE CXX)

add_executable(gendata ${CLI_SOURCES})
target_link_libraries(gendata libdatagen CLI11::CLI11)

add_executable(example example.cpp)
target_link_libraries(example libdatagen)

add_executable(tests tests/data.test.cpp)
target_link_libraries(tests PRIVATE libdatagen Catch2::Catch2WithMain)