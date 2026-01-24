#ifndef SCREEN_H
#define SCREEN_H

#include <ncurses.h>
#include <vector>

#define BORDER_SIZE 2
#define WIDTH_SCALER 2

class Screen {
  public:
    /* Constructor */
    Screen(int height, int width);
    void display(const std::vector<std::vector<bool>> &data);
  private:
    /* Height of the window */
    int _height;
    /* Width of the window */
    int _width;
    /* Pointer to the window */
    WINDOW *_window;
};

#endif