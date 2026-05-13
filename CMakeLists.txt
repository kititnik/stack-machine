cmake_minimum_required(VERSION 3.10)
project(vm C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(libs/stack)

add_library(vm
    src/vm.c
    src/parser.c
    src/executor.c
)

target_include_directories(vm
    PUBLIC include
)

target_link_libraries(vm
    PRIVATE stack
)