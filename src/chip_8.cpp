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

  /* Load font into memory starting at 0x50 */
  uint16_t ptr = FONT_MEMORY_ADDRESS;
  for(uint8_t font_data : font) {
    _memory[ptr++] = font_data;
  }
};

/* Decreases delay timer if bigger than 0 */
void Chip_8::decrease_delay_timer() {
  if(_delay_timer > 0) _delay_timer--;
};

/* Decreases sound timer if bigger than 0 */
void Chip_8::decrease_sound_timer() {
  if(_sound_timer > 0) _sound_timer--;
};