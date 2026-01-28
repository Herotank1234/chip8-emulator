#ifndef SCREEN_H
#define SCREEN_H

#include <vector>
#include <SFML/Graphics.hpp>

#define SCALE 10

class Screen {
  public:
    /* Constructor */
    Screen(u_int16_t height, u_int16_t width);
    /* Display the data to the screen */
    void display(const std::vector<std::vector<bool>> &data);
    /* Check if the window is still open */
    bool is_open();
  private:
    /* Height of the window */
    uint32_t _height;
    /* Width of the window */
    uint32_t _width;
    /* Pointer to the window */
    std::unique_ptr<sf::RenderWindow> _window;
    /* Pixel */
    sf::RectangleShape _square;
};

#endif
