add_subdirectory(components)
add_subdirectory(exceptions)
add_subdirectory(tools)

add_executable(${PROJECT_NAME}
        Main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE components exceptions tools nlohmann_json::nlohmann_json)
