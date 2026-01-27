#include <fstream>
#include <iostream>
#include <ncurses.h>
#include <unordered_map>
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

  /* Initialise screen */
  _screen = std::make_unique<Screen>(DISPLAY_HEIGHT, DISPLAY_WIDTH);

  /* Initialise random number generator */
  _mt = std::mt19937(_rand_dev());
  _uni_int_dist = std::uniform_int_distribution<std::mt19937::result_type>(0, 0xFF);

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
      /* 00EE - Return from subroutine */
      if(op2 == 0xE && op3 == 0xE) {
        /* Pop return address from stack */
        uint16_t return_addr = _stack.top();
        _stack.pop();

        /* Set pc to return address */
        _program_counter = return_addr;

      /* 00E0 - Clear screen instruction */
      } else if(op2 == 0xE && op3 == 0x0) {
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
    
    case 0x2:
      /* 2NNN - Call subroutine at location NNN */
      {
        /* Push current pc to stack */
        _stack.push(_program_counter);
        /* Get the address of the subroutine */
        uint16_t subroutine_addr = (op1 << (NIBBLE_SIZE * 2)) | (op2 << NIBBLE_SIZE) | op3;
        /* Set pc to the subroutine address */
        _program_counter = subroutine_addr;
      }
      break;

    case 0x3:
      /* 3XNN - Skip one instruction if vX == NN */
      if(_vs[op1] == second_byte) _program_counter += 2;
      break;

    case 0x4:
      /* 4XNN - Skip one instruction if vX != NN */
        if(_vs[op1] != second_byte) _program_counter += 2;
        break;

    case 0x5:
      /* 5XY0 - Skips one instruction is vX == vY */
      if(_vs[op1] == _vs[op2]) _program_counter += 2;
      break;
    
    case 0x6:
      /* 6XNN - Sets the register defined by X to the value defined by the last byte */
      _vs[op1] = second_byte;
      break;
    
    case 0x7:
      /* 7XNN - Adds the value defined by the last byte to the register defined by X */
      _vs[op1] += second_byte;
      break;

    case 0x8:
      switch(op3) {
        case 0x0:
          /* 8XY0 - vX = vY*/
          _vs[op1] = _vs[op2];
          break;
        
        case 0x1:
          /* 8XY1 - vX = vX | vY*/
          _vs[op1] = _vs[op1] | _vs[op2];
          break;

        case 0x2:
          /* 8XY2 - vX = vX & vY*/
          _vs[op1] = _vs[op1] & _vs[op2];
          break;

        case 0x3:
          /* 8XY3 - vX = vX ^ vY*/
          _vs[op1] = _vs[op1] ^ _vs[op2];
          break;

        case 0x4:
          {
            /* 8XY4 - vX += vY */
            uint initial_val = _vs[op1];
            _vs[op1] += _vs[op2];
            /* If overflow has happened, set the flag register to 1, otherwise 0 */
            _vs[FLAG_REG] = (_vs[op1] < initial_val ? 1 : 0);
          }
          break;

        case 0x5:
          {
            /* 8XY5 - vX = vX - vY */
            uint8_t initial_value = _vs[op1];
            _vs[op1] = _vs[op1] - _vs[op2];
            /* If underflow has happened, set flag register to 0, otherwise 1 */
            _vs[FLAG_REG] = (_vs[op1] > initial_value ? 0 : 1);
          }
          break;
        
        case 0x6:
          {
            /* 8XY6 - set vX = vY, then shift vX 1 bit to the right */
            _vs[op1] = _vs[op2];
            /* Get bit that will be shifted out */
            uint8_t shifted_bit = _vs[op1] & BIT_MASK;
            _vs[op1] >>= 1;
            /* Set flag register to the bit that was shifted out */
            _vs[FLAG_REG] = shifted_bit;
          }
          break;

        case 0x7:
          {
            /* 8XY7 - vX = vY - vX */
            uint8_t initial_value = _vs[op2];
            _vs[op1] = _vs[op2] - _vs[op1];
            /* If underflow has happened, set flag register to 0, otherwise 1 */
            _vs[FLAG_REG] = (_vs[op1] > initial_value ? 0 : 1);
          }
          break;

        case 0xE:
          {
            /* 8XYE - set vX = vY, then shift vX 1 bit to the left */
            _vs[op1] = _vs[op2];
            /* Get bit that will be shifted out */
            uint8_t shifted_bit = (_vs[op1] & (BIT_MASK << (BYTE_SIZE - 1))) >> (BYTE_SIZE - 1);
            _vs[op1] <<= 1;
            /* Set flag register to the bit that was shifted out */
            _vs[FLAG_REG] = shifted_bit;
          }
          break;
      }
      break;


    case 0x9:
      /* 9XY0 - Skips one instruction is vX != vY */
      if(_vs[op1] != _vs[op2]) _program_counter += 2;
      break;

    case 0xA:
      /* ANNN - Sets the index register to the value defined by the last three bytes */
      /* Concat the three opcodes*/
      {
        uint16_t new_index = (op1 << (NIBBLE_SIZE * 2)) | (op2 << NIBBLE_SIZE) | op3;
        _index_register = new_index;
      }
      break;
    
    case 0xB:
      /* BNNN - Jump to the address NNN + the value in v0 */
      {
        uint16_t new_address = (op1 << (NIBBLE_SIZE * 2)) | (op2 << NIBBLE_SIZE) | op3;
        new_address += _vs[0];
        _program_counter = new_address;
      }
      break;

    case 0xC:
      /* CXNN - Generates a random number, bitwise AND with the value NN and stores in vX */
      {
        uint8_t random_number = _uni_int_dist(_mt);
        random_number &= second_byte;
        _vs[op1] = random_number;
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

        Chip_8::display_to_screen();
      }
      break;
    
    case 0xE:
      switch(second_byte) {
        case 0x9E:
          /* EX9E - Skip one instruction if the key corresponding to the value in vX is pressed */
          {
            char c = _screen->get_char();
            std::unordered_map<char, uint8_t>::const_iterator it = keyboard_map.find(c);
            if(it != keyboard_map.end()) {
              uint8_t key_pressed = it->second;
              if(key_pressed == _vs[op1]) _program_counter += INSTRUCTION_SIZE;
            }
          }
          break;

        case 0xA1:
          /* EXA1 - Skip one instruction if the key corresponding to the value in vX is not pressed */
         {
            char c = _screen->get_char();
            std::unordered_map<char, uint8_t>::const_iterator it = keyboard_map.find(c);
            if(c == ERR || it == keyboard_map.end()) {
              _program_counter += INSTRUCTION_SIZE;
            } else {
              uint8_t key_pressed = it->second;
              if(key_pressed != _vs[op1]) _program_counter += INSTRUCTION_SIZE;
            }
          }
          break;
      }
      break;

    case 0xF:
      switch(second_byte) {
        case 0x0A:
          /* FX1A - Blocks until a character is pressed (by decrementing program counter) and sets vX to it */
          {
            char c = _screen->get_char();
            std::unordered_map<char, uint8_t>::const_iterator it = keyboard_map.find(c);
            if(it != keyboard_map.end()) {
              uint8_t key_pressed = it->second;
              _vs[op1] = key_pressed;
            } else {
              _program_counter -= INSTRUCTION_SIZE;
            }
          }
          break;

        case 0x07:
          /* Set vX to the value of the delay timer */
          _vs[op1] = _delay_timer;
          break;

        case 0x15:
          /* Set delay timer to the value in vX */
          _delay_timer = _vs[op1];
          break;

        case 0x18:
          /* Set sound timer to the value in vX */
          _sound_timer = _vs[op1];
          break;

        case 0x1E:
          /* FX1E - Add vX to index register */
          _index_register += _vs[op1];
          /* 
            If index register goes above 0x1000 (the normal addressing range), 
            set flag register 
          */
          if(_index_register > ADDRESS_RANGE) _vs[FLAG_REG] = 1;
          break;
      
        case 0x33:
          {
            /* 
              FX33 - Split number in vX into each digit and store in memory starting 
              at index register 
            */
            int num = _vs[op1];
            std::vector<int> digits;
            /* Split into units, tens and hundreds and store in digits (reversed) */
            for(int i = 0; i < 3; i++) {
              digits.push_back(num % 10);
              num /= 10;
            }

            /* Store into memory starting at index register */
            for(int i = 0; i < 3; i++) {
              _memory[_index_register + i] = digits[digits.size() - 1 - i];
            }
          }
          break;

        case 0x55:
          /* FX55 - Store registers v0 to vX into memory starting at index register */
          for(int i = 0; i <= op1; i++) {
            _memory[_index_register + i] = _vs[i];
          }
          break;

        case 0x65:
          /* FX65 - Load registers v0 to vX from memory starting at index register */
          for(int i = 0; i <= op1; i++) {
            _vs[i] = _memory[_index_register + i];
          }
          break;
      }
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
  /* Write the data to the screen */
  _screen->display(_display);
}
