#include <curses.h>
#include <string.h>
#include "globals.h"
#include "VarTable.h"
#include "string.h"
#include "fifo.h"
#include "interp.h"
#include "Word.h"

#define STRING_NAME "Enter your name:"
int init_mode = 0;
int desc_count = 0;

int runInit() {
  if (init_mode == 0) {
    wborder(screen, '|', '|', '_', '_', ' ', ' ', '|', '|');
    //
    char *name = NULL;
    name = setStringF(name, "%s %s", NAME, VERSION);
    mvaddstr(rows/2-5, cols/2-strlen(name)/2, name);
    free(name);
    mvaddstr(rows/2-4, cols/2-strlen(VNAME)/2, VNAME);
    init_mode = 1;

    mvaddstr(rows/2-2, cols/2-strlen(NOTE_1)/2, NOTE_1);
    mvaddstr(rows/2-1, cols/2-strlen(NOTE_2)/2, NOTE_2);

    char *any = "press any key to continue :)";
    mvaddstr(rows/2+1, cols/2-strlen(any)/2, any);
    refresh();
    getch();
    erase();

    int pos_y = 0;
    w_name = subwin(screen, 1, cols, pos_y, 0);
    wmove(w_name, 0, 0);
    pos_y++;
    w_desc = subwin(screen, rows-rows/4, cols-2, pos_y, 1);
    wmove(w_desc, 0, 0);
    w_opts = subwin(screen, rows/4, cols-2, rows-rows/4, 1);
    wmove(w_opts, 0, 0);
    //wrefresh(w_desc);
    //wrefresh(w_opts);
    refresh();

    // ask player for name
    wprintw(w_opts, "%s", STRING_NAME);
    wrefresh(w_opts);
    int ch;
    Word *word = newWord(16);
    while ((ch = getch()) != '\r') {
      if (ch == KEY_BACKSPACE) {
        word->string[word->pos--] = '\0';
      } else {
        addchar(word, ch);
      }
      mvwprintw(w_opts, 0, strlen(STRING_NAME)+1, "%s", word->string);
      wrefresh(w_opts);
    }
    werase(w_desc);
    wmove(w_desc, 0, 0);
    wprintw(w_desc, "Welcome, %s!\n", word->string);
    wrefresh(w_desc);
    getch();
    freeWord(word);
    //
    go_map = setStringF(go_map, "start");
    while(is_running) {
      char *filename = NULL;
      filename = setStringF(filename, "%s/%s.vad", "game", go_map);
      if (!fileExists(filename)) {
        werase(w_desc);
        wprintw(w_desc, "Could not load %s, press a key to quit :S\n", filename);
        wrefresh(w_desc);
        free(filename);
        getch();
        is_running = 0;
        break;
      }
      char *buf = NULL;
      int buf_size = readfile(&buf, filename);
      free(filename);
      filename = NULL;
      int ret;
      ret = parse_r(buf, 0, buf_size);
      free(buf);
      werase(w_desc);
      wmove(w_desc, 0, 0);
      if (ret == 10) {
        // exit to map
        is_running = 1;
        //wprintw(w_desc, "exiting to map");
      } else if (ret == 20) {
        werase(w_opts);
        wmove(w_opts, 0, 0);
        wprintw(w_opts, "<continue>");
        wrefresh(w_opts);
        getch();
      } else if (ret == 30) {
        werase(w_opts);
        wmove(w_opts, 0, 0);
        wprintw(w_opts, "<continue>");
        wrefresh(w_opts);
        getch();

        werase(w_desc);
        wprintw(w_desc, "It seems you've kicked the bucket in some manner or another - good luck next time!");
        wrefresh(w_desc);
        getch();
        is_running = 0;
      } else if (ret == 40) {
        werase(w_opts);
        wmove(w_opts, 0, 0);
        wprintw(w_opts, "<continue>");
        wrefresh(w_opts);
        getch();

        werase(w_desc);
        wprintw(w_desc, "Congratulations, victory is yours!");
        wrefresh(w_desc);
        getch();
        is_running = 0;
      }
    }
  } else {
    game_mode = AREA;
  }
  return 0;
}
