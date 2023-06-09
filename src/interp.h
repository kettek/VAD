#ifndef INTERP_H
#define INTERP_H
#include "VarTable.h"

typedef struct Scope {

} Scope;

typedef struct Pos {
  int begin;
  int end;
  int brk;
} Pos;
typedef struct PosList {
  int count;
  Pos *pos;
} PosList;
PosList *newPosList();
int freePosList(PosList *list);
int clearPosList(PosList *list);
int addPos(PosList *list, int start, int end);
int setPos(PosList *list, int start, int end);

// GLOBALS
VarTable *variables;

int readfile(char **buffer, const char *filename);
#define P_SEARCH 0
#define P_NAME 5
#define P_DESC_START 9
#define P_DESC 10
#define P_OPTS 20
#define P_OPTS_RUN 25
#define P_OPT 30
#define P_OPT_CLOSE 35
int parse_r(char *buffer, int start, int end);

#define M_NORM 0
#define M_SET 1
#define M_GO 2

#define C_NONE 0
#define C_CHECK 1
#define C_IGNORE 2
#define C_EAT_TO_FI 3
#define C_EAT_TO_ELSE 4

#endif
