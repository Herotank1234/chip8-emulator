cmake_minimum_required(VERSION 3.28)

project(chip_8_emulator)

file(GLOB_RECURSE CPP_FILES src/*.cpp)
file(GLOB_RECURSE H_FILES src/*.h)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(chip_8_emulator)

target_include_directories(chip_8_emulator PRIVATE src)

target_link_libraries(chip_8_emulator PRIVATE SFML::Graphics)

target_sources(chip_8_emulator PRIVATE ${CPP_FILES} ${H_FILES})

target_compile_options(chip_8_emulator PRIVATE -Wall -Wextra -std=c++17)