add_subdirectory(components)
add_subdirectory(exceptions)
add_subdirectory(tools)

add_executable(emulator
        Main.cpp
)

# Include JSON library
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(emulator PRIVATE components exceptions tools nlohmann_json::nlohmann_json)
