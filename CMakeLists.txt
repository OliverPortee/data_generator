
cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(data_generator)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB INC_SOURCES "include/data_generator/data_generator.hpp")
file(GLOB CLI_SOURCES "cli/main.cpp")

FetchContent_Declare(cli11 GIT_REPOSITORY https://github.com/CLIUtils/CLI11 GIT_TAG v2.2.0)
FetchContent_MakeAvailable(cli11)

FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.0.1)
FetchContent_MakeAvailable(Catch2)

add_library(libdatagen INTERFACE)
target_include_directories(libdatagen INTERFACE "include")
set_target_properties(libdatagen PROPERTIES LINKER_LANGUAGE CXX)

add_executable(gendata ${CLI_SOURCES})
target_link_libraries(gendata libdatagen CLI11::CLI11)
target_compile_options(gendata PRIVATE -Wall -Wextra -Werror -Wconversion -Wsign-conversion -Wpedantic)

add_executable(example example.cpp)
target_link_libraries(example libdatagen)

add_executable(tests tests/data.test.cpp)
target_link_libraries(tests libdatagen Catch2::Catch2WithMain)
target_compile_options(tests PRIVATE -Wall -Wextra -Werror -Wconversion -Wsign-conversion -Wpedantic)