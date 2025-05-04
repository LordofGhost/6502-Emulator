add_subdirectory(components)

add_executable(emulator
        Main.cpp
        Exception.cpp
        Tools.cpp
)

target_link_libraries(emulator PRIVATE components)