cmake_minimum_required(VERSION 3.8)
project(atlas_generator)

set(CMAKE_C_STANDARD 11)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")

set(SOURCE_FILES main.c)
add_executable(atlas_generator ${SOURCE_FILES})
