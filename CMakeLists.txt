cmake_minimum_required(VERSION 3.16)

project(chip_8_emulator)

add_executable(chip_8_emulator src/chip_8_emulator.cpp)

target_compile_options(chip_8_emulator PRIVATE -Wall -Wextra -std=c++17)