cmake_minimum_required(VERSION 3.16)

project(chip_8_emulator)

set(CURSES_NEED_NCURSES TRUE)

file(GLOB_RECURSE CPP_FILES src/*.cpp)
file(GLOB_RECURSE H_FILES src/*.h)
find_package(Curses REQUIRED)

add_executable(chip_8_emulator)

target_include_directories(chip_8_emulator PRIVATE src)

target_link_libraries(chip_8_emulator PRIVATE ${CURSES_LIBRARIES})

target_sources(chip_8_emulator PRIVATE ${CPP_FILES} ${H_FILES})

target_compile_options(chip_8_emulator PRIVATE -Wall -Wextra -std=c++17)