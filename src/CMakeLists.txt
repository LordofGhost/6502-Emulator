add_subdirectory(components)

add_executable(emulator
        Main.cpp
        Exception.cpp
)

target_link_libraries(emulator PRIVATE components)