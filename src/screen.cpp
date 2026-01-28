#include "screen.h"

Screen::Screen(uint16_t height, u_int16_t width) :_height(height), _width(width) {
  /* Create the window */
  _window = std::make_unique<sf::RenderWindow>(sf::VideoMode({_width * SCALE, _height * SCALE}), "Chip 8 Emulator");
  /* Create a rectangle shape as our pixel */
  _square = sf::RectangleShape({SCALE, SCALE});
}

void Screen::display(const std::vector<std::vector<bool>> &data) {
  _window->clear();
  for(uint16_t i = 0; i < _height; i++) {
    for(uint16_t j = 0; j < _width; j++) {
      if(data[i][j]) {
        _square.setPosition({j * SCALE, i * SCALE});
        _window->draw(_square);
      }
    }
  }
  _window->display();
}

bool Screen::is_open() {
  return _window->isOpen();
}

void Screen::poll_events() {
  while (const std::optional<sf::Event> event = _window->pollEvent()) {
    if (event->is<sf::Event::Closed>()) _window->close();
  }
}