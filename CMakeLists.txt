
cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(data_generator)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB INC_SOURCES "include/*.hpp")
file(GLOB CLI_SOURCES "cli/*.cpp")

FetchContent_Declare(cli11 GIT_REPOSITORY https://github.com/CLIUtils/CLI11 GIT_TAG v2.2.0)
FetchContent_MakeAvailable(cli11)

add_library(libdatagen ${INC_SOURCES})
target_include_directories(libdatagen PUBLIC "include")
set_target_properties(libdatagen PROPERTIES LINKER_LANGUAGE CXX)

add_executable(clidatagen ${CLI_SOURCES})
target_link_libraries(clidatagen libdatagen CLI11::CLI11)
