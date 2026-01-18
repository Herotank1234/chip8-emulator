#include <fstream>
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
  uint16_t ptr = FONT_ADDRESS;
  for(uint8_t font_data : font) {
    _memory[ptr++] = font_data;
  }

  /* Set to false as we have yet to write anything to the screen */
  _has_written = false;
};

/* Load ROM data into memory */
void Chip_8::load_ROM(std::string file_name) {
  /* Open file */
  std::ifstream file(file_name, std::ios_base::binary);

  /* Read data byte by byte and store in memory starting from 0x200 */
  char curr_byte;
  uint16_t ptr = PROGRAM_ADDRESS;
  while(file.get(curr_byte)) { 
    _memory[ptr++] = static_cast<uint8_t>(curr_byte);
  }
}

/* Decreases delay timer if bigger than 0 */
void Chip_8::decrease_delay_timer() {
  if(_delay_timer > 0) _delay_timer--;
};

/* Decreases sound timer if bigger than 0 */
void Chip_8::decrease_sound_timer() {
  if(_sound_timer > 0) _sound_timer--;
};

/* Runs one Fetch, decode, execute cycle */
void Chip_8::run_cycle() {
  /* Fetch - Read two successive bytes and increment PC by 2 */
  uint8_t first_byte = _memory[_program_counter++];
  uint8_t second_byte = _memory[_program_counter++];

  /* Decode - Bit mask the first nibble for the opcode, and last 3 nibbles for the operand */
  uint8_t opcode = (first_byte & FRONT_NIBBLE_MASK) >> NIBBLE_SIZE;

  uint8_t op1 = first_byte & BACK_NIBBLE_MASK;
  uint8_t op2 = (second_byte & FRONT_NIBBLE_MASK) >> NIBBLE_SIZE;
  uint8_t op3 = second_byte & BACK_NIBBLE_MASK ;

  /* Execute */
  switch(opcode) {
    case 0x0:
      /* 00E0 - Clear screen instruction */
      if(op2 == 0xE) {
        Chip_8::clear_screen_data();
        Chip_8::display_to_screen();
      }
      break;
    
    case 0x1:
      /* 1NNN - Sets the program counter to the value defined by the last three nibbles */
      /* Concat the three opcodes*/
      {
        uint16_t new_pc = (op1 << (NIBBLE_SIZE * 2)) | (op2 << NIBBLE_SIZE) | op3;
        _program_counter = new_pc;
      }
      break;
    
    case 0x6:
      /* 6XNN - Sets the register defined by X to the value defined by the last byte */
      _vs[op1] = second_byte;
      break;
    
    case 0x7:
      /* 7XNN - Adds the value defined by the last byte to the register defined by X */
      _vs[op1] += second_byte;
      break;

    case 0xA:
      /* ANNN - Sets the index register to the value defined by the last three bytes */
      /* Concat the three opcodes*/
      {
        uint16_t new_index = (op1 << (NIBBLE_SIZE * 2)) | (op2 << NIBBLE_SIZE) | op3;
        _index_register = new_index;
      }
      break;

    case 0xD:
      /* 
        DXYN - Draws sprite N tall starting at the coordinates (x, y) 
        from the registers defined by X and Y respectively 
      */
      /* Get x and y from their respective registers */
      {
        uint8_t x = _vs[op1] % DISPLAY_WIDTH;
        uint8_t y = _vs[op2] % DISPLAY_HEIGHT;

        /* Set flag register to 0 */
        _vs[FLAG_REG] = 0;

        for(uint16_t i = 0; i < static_cast<uint16_t>(op3); i++) {
          uint8_t sprite_data = _memory[_index_register + i];
          for(uint16_t j = 0; j < BYTE_SIZE; j++) {
            /* Get current sprite pixel */
            uint8_t sprite_pixel = (sprite_data & (BIT_MASK << (BYTE_SIZE - 1 - j))) >> (BYTE_SIZE - 1 - j);
            uint8_t display_pixel = _display[y + i][x + j];

            /* If they are both on then set vF to 1 */
            if(sprite_pixel && display_pixel) _vs[FLAG_REG] = 1;

            /* Set display pixel to the XOR of both pixels */
            _display[y + i][x + j] = sprite_pixel ^ display_pixel;

            /* If the right edge of the screen is reached, stop drawing this row */
            if(x + j == DISPLAY_WIDTH - 1) break;
          }

          /* If the bottom edge of the screen is reached, stop */
          if(y + i == DISPLAY_HEIGHT - 1) break;
        }
      }
      Chip_8::display_to_screen();
      break;
  }
}

/* Sets all values held in _display to false */
void Chip_8::clear_screen_data() {
  for(size_t i = 0; i < DISPLAY_HEIGHT; i++) {
    for(size_t j = 0; j < DISPLAY_WIDTH; j++) {
      _display[i][j] = false;
    }
  }
}

/* Writes all data held in _display to the screen */
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