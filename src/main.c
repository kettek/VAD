#include <curses.h>
#include <stdio.h>
#include "globals.h"

int main(int argc, char *argv[]) {
  // ** INIT **
  // set up display
  if ((screen = initscr()) == NULL) {
    perror("initscr() error'd");
    return 1;
  }
  curs = curs_set(0); // store original cursor position for restore/hide of cursor
  noecho(); // disable key echo
  nonl(); // no NL->CR/NL, we'll handle that ourselves
  keypad(screen, TRUE); // enable arrow keys/keypad
  cbreak(); // Handle characters immediately.
  // if we have colors, set up all possible color pairs (up to COLORS)
  if (has_colors()) {
    start_color();
    int x, y;
    for (x = 0; x < COLORS; x++) {
      for (y = 0; y < COLORS; y++) {
        init_pair(x*COLORS+y, x, y);
      }
    }
  }
  // get term dimensions
  cols = getmaxx(screen);
  rows = getmaxy(screen);
  // ** PROGRAM LOOP **
  global_variables = newVarTable(128);
  special_variables = newVarTable(32);
  game_mode = INIT;
  is_running = 1;
  go_map = NULL;
  while(is_running) {
    switch(game_mode) {
      case INIT:
        runInit();
        break;
      case BEGIN:
        break;
      case MENU:
        //runMenu();
        break;
      case MAP:
        //runMap();
        break;
      case AREA:
        //runArea();
        break;
      case END:
        break;
    }
  }
  wrefresh(screen);
  delwin(screen);
  endwin();
  return 0;
}
