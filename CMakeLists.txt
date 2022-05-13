
cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(data_generator)
set(CMAKE_CXX_STANDARD 20)

file(GLOB LIB_SOURCES "lib/*.cpp")
file(GLOB INC_SOURCES "include/*.cpp")
file(GLOB CLI_SOURCES "cli/*.cpp")

FetchContent_Declare(cxxopts GIT_REPOSITORY https://github.com/jarro2783/cxxopts)
FetchContent_MakeAvailable(cxxopts)

add_library(libdatagen ${LIB_SOURCES} ${INC_SOURCES})
target_include_directories(libdatagen PUBLIC "include")

add_executable(clidatagen ${CLI_SOURCES})
target_link_libraries(clidatagen libdatagen cxxopts)
