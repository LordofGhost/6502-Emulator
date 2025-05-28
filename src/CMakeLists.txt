add_subdirectory(components)
add_subdirectory(exceptions)
add_subdirectory(tools)

add_executable(emulator
        Main.cpp
)

target_link_libraries(emulator PRIVATE components exceptions tools)
