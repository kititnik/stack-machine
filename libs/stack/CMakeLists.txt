cmake_minimum_required(VERSION 3.10)
project(stack_lib C)

add_library(stack
    src/stack.c
)

target_include_directories(stack
    PUBLIC include
)