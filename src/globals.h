#ifndef GLOBALS_H
#define GLOBALS_H
#define NAME "VAD"
#define VERSION "0.1"
#define VNAME "\"they fragile as eggs\""
#define NOTE_1 "cobbled together by kts_kettek"
#define NOTE_2 "game testing and implementation by vexman"
#include "VarTable.h"
#include "Word.h"
#include <curses.h>

WINDOW *screen;
int is_running;
int cols;
int rows;
int curs;

int desc_count;

VarTable *global_variables;
VarTable *special_variables;
char *go_map;

int game_mode;
#define INIT 0
int runInit();
#define MENU 1
int runMenu();
#define BEGIN 2
int runBegin();
#define MAP 3
int runMap();
#define AREA 4
int runArea();
#define END 5
int runEnd();

WINDOW *w_name;
WINDOW *w_desc;
WINDOW *w_opts;
int handleOptions(WordList *words);
int reinitDisplay();
int renderDesc(const char *string);
int renderName(const char *string);

#endif
