#include "chip_8.h"

Chip_8::Chip_8() {
  /* Initialise zeroed out memory */
  _memory = std::vector<uint8_t>(MEMORY_SIZE, 0);

  /* Initialise display with all pixels off */
  _display = std::vector<std::vector<bool>>(DISPLAY_HEIGHT, 
    std::vector<bool>(DISPLAY_WIDTH, false));

  /* Set various counters, registers, timers to 0 and initialise stack */
  _program_counter = 0;
  _index_register = 0;
  _stack = std::stack<uint16_t>();
  _delay_timer = 0;
  _sound_timer = 0;
  _vs = std::vector<uint8_t>(NUMBER_OF_GENERAL_REGISTERS, 0);
}