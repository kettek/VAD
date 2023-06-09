#include "globals.h"
#include "Word.h"

int renderName(const char *string) {
  reinitDisplay();
  werase(w_name);
  wmove(w_name, 0, 0);
  wprintw(w_name, "%s", string);
  wrefresh(w_name);
  desc_count--;
  return 0;
}

int handleOptions(WordList *words) {
  reinitDisplay();
  Word *opt;
  int i = 0;
  int y = 1;
  int l = 1;
  while(l) {
    werase(w_opts);
    wmove(w_opts, 0, 0);
    i = 0;
    while((opt = readWordList(words)) != NULL) {
      i++;
      if (i == y) {
        wattron(w_opts, COLOR_PAIR(2));
        wprintw(w_opts, "%s\n", opt->string);
        wattroff(w_opts, COLOR_PAIR(2));
      } else {
        wprintw(w_opts, "%s\n", opt->string);

      }
      winsertln(w_opts);
    }
    wrefresh(w_opts);
    int ch = getch();
    switch(ch) {
      case KEY_UP:
        y--;
        if (y < 1) y = 1;
        break;
      case KEY_DOWN:
        y++;
        if (y > i) y = i;
        break;
      case '\r':
      case '\n':
        l = 0;
        break;
    }
  }
  wprintw(w_opts, "%d", y);
  winsertln(w_opts);
  return y-1;
}

int renderDesc(const char *string) {
  reinitDisplay();
  if (desc_count >= 1) {
    werase(w_opts);
    wmove(w_opts, 0, 0);
    wprintw(w_opts, "<continue>");
    wrefresh(w_opts);
    getch();
    desc_count = 0;
  }
  werase(w_desc);
  wmove(w_desc, 0, 0);

  wprintw(w_desc, "%s", string);
  wrefresh(w_desc);
  desc_count++;
  //freeWord(word);
  return 0;
}

int reinitDisplay() {
  int new_cols = getmaxx(screen);
  int new_rows = getmaxy(screen);
  if (new_rows != rows || new_cols != cols) {
    rows = new_rows;
    cols = new_cols;

    wresize(w_name, 1, cols);
    wresize(w_desc, new_rows-new_rows/4, new_cols-1);
    wresize(w_opts, new_rows/4, new_cols-1);
    mvwin(w_opts, new_rows-new_rows/4, 0);
    wrefresh(w_name);
    wrefresh(w_desc);
    wrefresh(w_opts);
  }
  return 0;
}
