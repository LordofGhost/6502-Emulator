include(FetchContent)

FetchContent_Declare(
        Catch2
        URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.7.1.tar.gz
)
FetchContent_MakeAvailable(Catch2)

file(GLOB_RECURSE CPU_INSTRUCTION_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/cpu/*.cpp"
)

add_executable(cpu_instruction_tests
        ${CPU_INSTRUCTION_TEST_SOURCES}
        support/W65C02TestHarness.cpp
        ${CMAKE_SOURCE_DIR}/src/components/cpu/W65C02.cpp
        ${CMAKE_SOURCE_DIR}/src/exceptions/EmulatorException.cpp
)

target_compile_definitions(cpu_instruction_tests PRIVATE EMULATOR_TESTING)
target_include_directories(cpu_instruction_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(cpu_instruction_tests PRIVATE Catch2::Catch2WithMain)

add_test(NAME cpu_instruction_tests COMMAND cpu_instruction_tests)
