#include <iostream>
#include "chip_8.h"

Chip_8::Chip_8() {
  /* Initialise zeroed out memory */
  _memory = std::vector<uint8_t>(MEMORY_SIZE, 0);

  /* Initialise display with all pixels off */
  _display = std::vector<std::vector<bool>>(DISPLAY_HEIGHT, 
    std::vector<bool>(DISPLAY_WIDTH, false));

  /* Set various counters, registers, timers to 0 and initialise stack */
  _program_counter = PROGRAM_ADDRESS;
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

  /* Set to false as we have yet to write anything to the screen */
  _has_written = false;
};

/* Decreases delay timer if bigger than 0 */
void Chip_8::decrease_delay_timer() {
  if(_delay_timer > 0) _delay_timer--;
};

/* Decreases sound timer if bigger than 0 */
void Chip_8::decrease_sound_timer() {
  if(_sound_timer > 0) _sound_timer--;
};

void Chip_8::run_cycle() {
  /* Fetch - Read two successive bytes and increment PC by 2 */
  uint8_t first_byte = _memory[_program_counter++];
  uint8_t second_byte = _memory[_program_counter++];

  /* Decode - Bit mask the first nibble for the opcode, and last 3 nibbles for the operand */
  uint8_t opcode = (first_byte & FRONT_NIBBLE_MASK) >> NIBBLE_SIZE;

  uint8_t op1 = first_byte & BACK_NIBBLE_MASK;
  uint8_t op2 = (second_byte & FRONT_NIBBLE_MASK) >> NIBBLE_SIZE;
  uint8_t op3 = first_byte & BACK_NIBBLE_MASK ;

  /* Execute */

  switch(opcode) {
    case 0x0:
      /* 00E0 - Clear screen instruction */
      if(op2 == 0xE) {
        Chip_8::clear_screen_data();
        Chip_8::display_to_screen();
      }
      break;
  }
}

void Chip_8::clear_screen_data() {
  for(size_t i = 0; i < DISPLAY_HEIGHT; i++) {
    for(size_t j = 0; j < DISPLAY_WIDTH; j++) {
      _display[i][j] = false;
    }
  }
}

void Chip_8::display_to_screen() {
  /* If we have already written to the screen, move the cursor back to the top */
  if(_has_written) { for(int i = 0; i < DISPLAY_HEIGHT; i++) std::cout << "\x1b[A"; }

  /* Write the data to the screen */
  for(std::vector<bool> row : _display) {
    /* Move the cursor back to the start of the line and write the new data */
    std::cout << "\r " << std::flush;
    for(bool pixel : row) {
      std::cout << (pixel ? "#" : ".") << " ";
    }
    std::cout << "\n";
  }

  /* Set _has_written if it already has not been set */
  if(!_has_written) _has_written = true;
}