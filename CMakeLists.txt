cmake_minimum_required(VERSION 3.29)
project(Emulator LANGUAGES C CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v -g3 -fno-omit-frame-pointer")

# Copy log config file
file(COPY ${CMAKE_SOURCE_DIR}/logsConfig.json DESTINATION ${CMAKE_BINARY_DIR}/src)

add_subdirectory(src)