#include <chrono>
#include <memory>
#include <string>
#include "chip_8.h"
#include "screen.h"

#define TIMER_FRAME_DURATION 16.666
#define CYCLE_FRAME_DURATION 1.428

int main(int argc, char** argv) {
  std::unique_ptr<Chip_8> chip_8 = std::make_unique<Chip_8>();
  std::unique_ptr<Screen> screen = std::make_unique<Screen>(DISPLAY_HEIGHT, DISPLAY_WIDTH);

  /* Load the ROM */
  std::string file_name = argv[1];
  chip_8->load_ROM(file_name);

  /* Get the current time */
  std::chrono::system_clock::time_point prev_timer_time = std::chrono::system_clock::now();
  std::chrono::system_clock::time_point prev_cycle_time = std::chrono::system_clock::now();
  std::chrono::system_clock::time_point curr_time = std::chrono::system_clock::now();

  while(screen->is_open()) {
    curr_time = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> cycle_time_passed = curr_time - prev_cycle_time;
    std::chrono::duration<double, std::milli> timer_time_passed = curr_time - prev_timer_time;

    /* 
      If enough time has passed since we lasted checked, run one cycle
      (Around 700 times per second) 
    */
    if(cycle_time_passed.count() >= CYCLE_FRAME_DURATION) {
      prev_cycle_time = curr_time;
      screen->poll_events();
      chip_8->update_keyboard_status();
      chip_8->run_cycle();
    }

    /* 
      If enough time has passed since we lasted checked, decrease both timers
      and update screen 
      (Decrease around 60 times per second) 
    */
    if(timer_time_passed.count() >= TIMER_FRAME_DURATION) {
      prev_timer_time = curr_time;
      chip_8->decrease_delay_timer();
      chip_8->decrease_sound_timer();
      screen->display(chip_8->get_data());
    }
  }

  return 0;
}
