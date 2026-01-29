#ifndef CHIP_8_H
#define CHIP_8_H

#include <random>
#include <SFML/Window.hpp>
#include <stack>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

#define MEMORY_SIZE 4096

#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 64

#define NUMBER_OF_GENERAL_REGISTERS 16

#define FONT_ADDRESS 0x50
#define PROGRAM_ADDRESS 0x200
#define ADDRESS_RANGE 0x1000

#define BIT_MASK 0x1
#define FRONT_NIBBLE_MASK 0xF0
#define BACK_NIBBLE_MASK 0x0F

#define NIBBLE_SIZE 4
#define BYTE_SIZE 8
#define INSTRUCTION_SIZE 2

#define FLAG_REG 0xF

#define NO_KEY 0xFF

const std::vector<uint8_t> font = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, 
  0x20, 0x60, 0x20, 0x20, 0x70, 
  0xF0, 0x10, 0xF0, 0x80, 0xF0, 
  0xF0, 0x10, 0xF0, 0x10, 0xF0, 
  0x90, 0x90, 0xF0, 0x10, 0x10, 
  0xF0, 0x80, 0xF0, 0x10, 0xF0, 
  0xF0, 0x80, 0xF0, 0x90, 0xF0, 
  0xF0, 0x10, 0x20, 0x40, 0x40, 
  0xF0, 0x90, 0xF0, 0x90, 0xF0, 
  0xF0, 0x90, 0xF0, 0x10, 0xF0, 
  0xF0, 0x90, 0xF0, 0x90, 0x90, 
  0xE0, 0x90, 0xE0, 0x90, 0xE0, 
  0xF0, 0x80, 0x80, 0x80, 0xF0, 
  0xE0, 0x90, 0x90, 0x90, 0xE0, 
  0xF0, 0x80, 0xF0, 0x80, 0xF0, 
  0xF0, 0x80, 0xF0, 0x80, 0x80
};

const std::unordered_map<sf::Keyboard::Key, uint8_t> keyboard_mapping = {
  {sf::Keyboard::Key::Num1, 0x1},
  {sf::Keyboard::Key::Num2, 0x2},
  {sf::Keyboard::Key::Num3, 0x3},
  {sf::Keyboard::Key::Num4, 0xC},
  {sf::Keyboard::Key::Q, 0x4},
  {sf::Keyboard::Key::W, 0x5},
  {sf::Keyboard::Key::E, 0x6},
  {sf::Keyboard::Key::R, 0xD},
  {sf::Keyboard::Key::A, 0x7},
  {sf::Keyboard::Key::S, 0x8},
  {sf::Keyboard::Key::D, 0x9},
  {sf::Keyboard::Key::F, 0xE},
  {sf::Keyboard::Key::Z, 0xA},
  {sf::Keyboard::Key::X, 0x0},
  {sf::Keyboard::Key::C, 0xB},
  {sf::Keyboard::Key::V, 0xF}
};

class Chip_8 {
  public:
    /* Construtor*/
    Chip_8();
    /* Load ROM data into memory */
    void load_ROM(std::string);
    /* Decreases delay timer by 1 if its value is bigger than 0 */
    void decrease_delay_timer();
    /* Decreases sound timer by 1 if its value is bigger than 0 */
    void decrease_sound_timer();
    /* Run the fetch, decode and execute cycle */
    void run_cycle();
    /* Clears the display data by turning all values to false */
    void clear_screen_data();
    /* Get the data stored in _display */
    std::vector<std::vector<bool>> get_data();
    /* Updates the status of the keyboard */
    void update_keyboard_status();
  private:
    /* Memory - 4KB */
    std::vector<uint8_t> _memory;
    /* Display - 64x32 pixels */
    std::vector<std::vector<bool>> _display;
    /* Program counter */
    uint16_t _program_counter;
    /* Index register */
    uint16_t _index_register;
    /* Stack for 16-bit addresses */
    std::stack<uint16_t> _stack;
    /* Delay timer */
    uint8_t _delay_timer;
    /* Sound timer */
    uint8_t _sound_timer;
    /* 16 8-bit general registers named v0 to vF */
    std::vector<uint8_t> _vs;
    /* Value to keep track of whether we have written to the screen before */
    bool _has_written;
    /* Random number generation */
    std::random_device _rand_dev;
    std::mt19937 _mt;
    std::uniform_int_distribution<std::mt19937::result_type> _uni_int_dist;
    /* Chip 8 Keyboard */
    std::unordered_map<uint8_t, bool> _keyboard;
    uint8_t _curr_pressed_key;
};

#endif
