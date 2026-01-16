#ifndef CHIP_8_H
#define CHIP_8_H

#include <stack>
#include <stdint.h>

#define MEMORY_SIZE 4096
#define DISPLAY_HEIGHT 32
#define DISPLAY_HEIGHT 64

class chip_8 {
  public:
  private:
    /* Memory - 4KB */
    uint8_t *memory;
    /* Display - 64x32 pixels */
    bool **display;
    /* Program counter */
    uint16_t program_counter;
    /* Index register */
    uint16_t index_register;
    /* Stack for 16-bit addresses */
    std::stack<uint16_t> stack;
    /* Delay timer */
    uint8_t delay_timer;
    /* Sound timer */
    uint8_t sound_timer;
    /* 16 8-bit general registers */
    uint8_t *vs;
};

#endif