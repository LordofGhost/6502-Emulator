cmake_minimum_required(VERSION 3.29)
project(Emulator LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v -g3 -fno-omit-frame-pointer")

add_subdirectory(src)