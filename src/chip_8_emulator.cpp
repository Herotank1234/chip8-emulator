#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include "chip_8.h"
#include "screen.h"

#define TIMER_FRAME_DURATION 16.666
#define CYCLE_FRAME_DURATION 0.1

std::optional<Arguments> parse_arguments(int argc, char **argv) {
  /* Descriptions of the optional flags a user can provide */
  boost::program_options::options_description description("Options");
  description.add_options()
    ("help", "Print help message and exit")
    ("input-file", boost::program_options::value<std::string>(), "Specify the path of the ROM to be loaded")
    ("dw", "Sets display waiting to off (default: on)")
    ("vfreset", "AND, OR, XOR reset flag register to 0 to off (default: on)")
    ("meminc", "Increments index register when loading from and storing to memory to off (default: on)")
    ("noclip", "Clip sprite at edge of screen to off (default: on)")
    ("shiftx", "Shift operations will only affect register x to on (default off)");
  /* Make the input-file flag optional, user can provide a file name only without using the input-file flag */
  boost::program_options::positional_options_description pod;
  pod.add("input-file", -1);
  boost::program_options::variables_map variables_map;

  boost::program_options::store(
    boost::program_options::command_line_parser(argc, argv) 
      .options(description)
      .positional(pod)
      .run(),
    variables_map
  );

  boost::program_options::notify(variables_map);
  
  std::string file_name;

  /* Check for the help flag */
  if(variables_map.count("help")) {
    std::cout << "Usage: ./chip_8_emulator [OPTIONS] <PATH-TO-ROM>" << std::endl;
    std::cout << std::endl;
    std::cout << description << std::endl;
    return {};
  }

  Arguments args{"", true, true, true, true, false};

  /* Check for the input-file flag */
  if(!variables_map.count("input-file")) {
    std::cout << "Please provide a path to a Chip 8 ROM" << std::endl;
    std::cout << "Use --help for more info" << std::endl;
    return {};
  } else {
    args.file_name = variables_map["input-file"].as<std::string>();
  }

  if(variables_map.count("dw")) {
    args.dw = false;
  }

  if(variables_map.count("vfreset")) {
    args.vfreset = false;
  }

  if(variables_map.count("meminc")) {
    args.meminc = false;
  }

  if(variables_map.count("noclip")) {
    args.clip = false;
  }
  
  if(variables_map.count("shiftx")) {
    args.shiftx = true;
  }

  return {args};
}

int main(int argc, char **argv) {
  /* Parse command line arguments */
  std::optional<Arguments> opt_arguments = parse_arguments(argc, argv);
  /* If the command line arguments parsing failed, return from the program */
  if(!opt_arguments) return 0;
  Arguments args = *opt_arguments;

  std::unique_ptr<Chip_8> chip_8 = std::make_unique<Chip_8>(args);
  /* Load the ROM and check if it was successful */
  bool success = chip_8->load_ROM();
  if(!success) {
    std::cout << args.file_name << " could not be opened. Check "
      "if this file exists and the path supplied is correct" << std::endl;
    return 0;
  }

  std::unique_ptr<Screen> screen = std::make_unique<Screen>(DISPLAY_HEIGHT, DISPLAY_WIDTH);

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
      chip_8->set_refresh_state();
    }
  }

  return 0;
}
