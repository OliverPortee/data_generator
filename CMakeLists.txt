
cmake_minimum_required(VERSION 3.13)

project(data_generator)
set(CMAKE_CXX_STANDARD 20)

file(GLOB LIB_SOURCES "lib/*.cpp")
file(GLOB INC_SOURCES "include/*.cpp")
file(GLOB CLI_SOURCES "cli/*.cpp")

add_library(libdatagen ${LIB_SOURCES} ${INC_SOURCES})
target_include_directories(libdatagen PUBLIC "include")


add_executable(clidatagen ${CLI_SOURCES})
target_link_libraries(clidatagen libdatagen)
