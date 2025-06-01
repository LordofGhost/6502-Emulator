add_library(tools
        Convert.cpp
        Logs.cpp
)

# Include JSON library
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)
target_include_directories(tools PRIVATE ${CMAKE_BINARY_DIR}/_deps/json-src/include)
