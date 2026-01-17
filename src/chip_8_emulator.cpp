#include <chrono>
#include <iostream>
#include <memory>
#include "chip_8.h"

#define TIMER_FRAME_DURATION 16.66

int main() {
  std::unique_ptr<Chip_8> chip_8(new Chip_8());

  /* Display the empty screen */
  chip_8->display_to_screen();

  /* Get the current time */
  std::chrono::system_clock::time_point prev_timer_time = std::chrono::system_clock::now();
  std::chrono::system_clock::time_point curr_timer_time = std::chrono::system_clock::now();

  while(true) {
    curr_timer_time = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> time_passed = curr_timer_time - prev_timer_time;

    /* 
      If enough time has passed since we lasted checked, decrease both timers 
      (Decrease around 60 times per second) 
    */
    if(time_passed.count() >= TIMER_FRAME_DURATION) {
      prev_timer_time = curr_timer_time;
      chip_8->decrease_delay_timer();
      chip_8->decrease_sound_timer();
    }
  }

  return 0;
}