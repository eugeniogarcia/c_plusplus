cmake_minimum_required (VERSION 3.8)

project ("hola")

# Add source to this project's executable.
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBS OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable (hola "hola.cpp" "hola.h")
