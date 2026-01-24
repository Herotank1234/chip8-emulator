#include "screen.h"

Screen::Screen(int height, int width) :_height(height), _width(width) {
  /* Initialise the screen */
  initscr();
  /* Allow the screen to use color */
  start_color();
  /* Don't echo user input */
  noecho();
  /* Set the cursor to inivisible */
  curs_set(0);
  /* Display to screen */
  refresh();

  /* Initilise the pairs of colors we can use to highlight text */
  init_pair(1, COLOR_BLACK, COLOR_WHITE);

  int row;
  int col;
  /* Get the size of the current screen */
  getmaxyx(stdscr, row, col);

  /* Find the coordinates of the top left of where the window should be drawn */
  int top_left_x = (col - (_width * WIDTH_SCALER) - BORDER_SIZE) / 2;
  int top_left_y = (row - _height - BORDER_SIZE) / 2;

  /* Create a new window from the dimension calculated from above */
  _window = newwin(_height + BORDER_SIZE, (_width * WIDTH_SCALER) + BORDER_SIZE, 
    top_left_y, top_left_x);

  /* Create a border around our window */
  box(_window, 0, 0);

  /* Display to window */
  wrefresh(_window);
}

void Screen::display(const std::vector<std::vector<bool>> &data) {
  for(int i = 0; i < _height; i++) {
    for(int j = 0; j < _width; j++) {
      /* Switch the highlight to white if the pixel is on */
      if(data[i][j]) wattron(_window, COLOR_PAIR(1));
      /* Write data to the screen */
      mvwprintw(_window, i + 1, (j * 2) + 1, "%s", "  ");
      /* Switch the highlight back if it was switched before */
      if(data[i][j]) wattroff(_window, COLOR_PAIR(1));
    }
  }
  
  /* Display to window*/
  wrefresh(_window);
}