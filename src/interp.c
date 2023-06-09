#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interp.h"
#include "Word.h"
#include "globals.h"
#include "string.h"

PosList *newPosList() {
  PosList *list = malloc(sizeof(PosList));
  list->count = 0;
  list->pos = NULL;
  return list;
}
int freePosList(PosList *list) {
  if (list->pos != NULL) {
    free(list->pos);
  }
  free(list);
  return 0;
}
int clearPosList(PosList *list) {
  if (list->pos != NULL) {
    free(list->pos);
    list->pos = NULL;
  }
  list->count = 0;
  return 0;
}
int addPos(PosList *list, int begin, int end) {
  list->pos = realloc(list->pos, (list->count+1)*sizeof(Pos));
  list->pos[list->count].begin = begin;
  list->pos[list->count].end = end;
  list->pos[list->count].brk = 0;
  list->count++;
  return 0;
}
/* readfile
This function reads the given filename into the passed dst buffer
*/
int readfile(char **buffer, const char *filename) {
  int buffer_pos = 0; // buffer position
  //*buffer = realloc(*buffer, )
  char r_buf[128]; // read buffer
  FILE *file = fopen(filename, "r");
  int r_bytes; // current bytes read
  while((r_bytes = fread(r_buf, 1, 127, file)) > 0) {
    *buffer = realloc(*buffer, buffer_pos+r_bytes);
    memcpy(*buffer+buffer_pos, r_buf, r_bytes);
    buffer_pos += r_bytes;
  }
  *buffer = realloc(*buffer, buffer_pos+1);
  (*buffer)[buffer_pos++] = '\0';
  fclose(file);
  return buffer_pos;
}

/* parse_r
This function makes up the bulk of the interpreter. It parses the given file's logic recursively, travelling as deep as it needs.
*/

int parse_r(char *buffer, int start, int end) {
  //printf("reading from %d to %d\n", start, end);
  int i = start;
  Word *search = newWord(32); // word, used to store current operating word
  Word *line = newWord(32); // used to store the current line
  Word *word = newWord(32); // used to store temp search word
  Word *meta = newWord(32); // meta wordz
  Word *cond = newWord(32); // for more spaghetti code
  WordList *options = newWordList(8); // used to store list options
  PosList *options_pos = newPosList(); // used to store ranges for ^
  int loop_i = 0; // current loop depth, -1 is none
  int p_i = 0;
  int a = 0;
  //int b = 0;
  int mode = P_SEARCH;
  int meta_mode = M_NORM;
  int cond_mode = C_NONE;
  int opt_depth = 0;
  char ch;
  // TODO: fix buffer overflow
  while ((ch = buffer[i++]) != '\0') {
    if (i > end) break;
    // BEGIN IF CHECKING only if we're not reading opt(s)!
    if (mode != P_OPT && mode != P_OPT_CLOSE) {
      if (cond_mode == C_NONE) {
        if (ch == ' ' || ch == '\t') {
          if (isWord(cond, ".if")) {
            cond_mode = C_CHECK;
          } else if (isWord(cond, ".elif")) {
            cond_mode = C_EAT_TO_FI;
          }
          clearWord(cond);
        } else if (ch == '\n') {
          if (isWord(cond, ".else")) {
            cond_mode = C_EAT_TO_FI;
          }
          clearWord(cond);
        } else {
          addchar(cond, ch);
        }
        // if 'if' == false, ignore all chars until .else/.elif/.fi
        // if 'if' == true, parse as normal
      } else if (cond_mode == C_CHECK) {
        if (ch != ' ' && ch != '\t' && ch != '\n') {
          addchar(cond, ch);
        } else if (ch == '\n') {
          int check = checkVar(global_variables, cond->string);
          if (check == 0) {
            // FALSE
            // eat until we find an .else, .elif, or .fi
            cond_mode = C_EAT_TO_ELSE;
          } else {
            // TRUE
            // run normal until we find a .fi
            clearWord(cond);
            cond_mode = C_NONE;
          }
        }
      } else if (cond_mode == C_EAT_TO_ELSE) {
        while(1) {
          ch = buffer[i++];
          if (ch == ' ' || ch == '\t') {
            if (isWord(cond, ".elif")) {
              //printf("found elif\n");
              cond_mode = C_CHECK;
              clearWord(cond);
              break;
            }
            clearWord(cond);
          } else if (ch == '\n') {
            if (isWord(cond, ".else")) {
              cond_mode = C_NONE;
              clearWord(cond);
              break;
            } else if (isWord(cond, ".fi")) {
              cond_mode = C_NONE;
              clearWord(cond);
              break;
            }
            clearWord(cond);
          } else {
            addchar(cond, ch);
          }
        }
      } else if (cond_mode == C_EAT_TO_FI) {
        while(1) {
          ch = buffer[i++];
          if (ch != ' ' && ch != '\t' && ch != '\n') {
            addchar(cond, ch);
          } else if (ch == '\n') {
            if (isWord(cond, ".fi")) {
              cond_mode = C_NONE;
              break;
            }
            clearWord(cond);
          }
        }
      }
    }
    // END IF CHECKING
    // BEGIN META CHECKING, such as .set, .exit, .break, etc.
    if (meta_mode == M_NORM) {
      if (ch == ' ' || ch == '\t') {
        if (mode == P_SEARCH) {
          if (isWord(meta, ".set")) {
            clearWord(meta);
            meta_mode = M_SET;
          } else if (isWord(meta, ".go")) {
            clearWord(meta);
            meta_mode = M_GO;
          }
        }
      } else if (ch != '\n') {
        addchar(meta, ch);
      } else {
        if (mode == P_SEARCH) {
          if (isWord(meta, ".break")) {
            freeWord(meta);
            freeWord(cond);
            freeWord(search);
            freeWord(word);
            freeWord(line);
            freeWordList(options);
            freePosList(options_pos);
            return 1;
          } else if (isWord(meta, ".exit")) {
            freeWord(meta);
            freeWord(cond);
            freeWord(search);
            freeWord(word);
            freeWord(line);
            freeWordList(options);
            freePosList(options_pos);
            return 10;
          } else if (isWord(meta, ".die")) {
            freeWord(meta);
            freeWord(cond);
            freeWord(search);
            freeWord(word);
            freeWord(line);
            freeWordList(options);
            freePosList(options_pos);
            return 30;
          } else if (isWord(meta, ".win")) {
            freeWord(meta);
            freeWord(cond);
            freeWord(search);
            freeWord(word);
            freeWord(line);
            freeWordList(options);
            freePosList(options_pos);
            return 40;
          }
        }
        clearWord(meta);
      }
    } else if (meta_mode == M_SET) {
      if (ch != '\n') {
        addchar(meta, ch);
      } else {
        interpVar(global_variables, meta->string);
        meta_mode = M_NORM;
        clearWord(meta);
      }
    } else if (meta_mode == M_GO) {
      if (ch != '\n') {
        addchar(meta, ch);
      } else {
        go_map = setStringF(go_map, meta->string);
        freeWord(meta);
        freeWord(cond);
        freeWord(search);
        freeWord(word);
        freeWord(line);
        freeWordList(options);
        freePosList(options_pos);
        return 20;
      }

    }
    // END META CHECKING
    // BEGIN normal operations
    switch(mode) {
      case P_SEARCH:
        if (ch == ' ' || ch == '\t') continue;
        addchar(search, ch);
        if (isWord(search, ".desc")) {
          clearWord(search);
          mode = P_DESC;
        } else if (isWord(search, ".name")) {
          clearWord(search);
          mode = P_NAME;
        } else if (isWord(search, ".list")) {
          // record start of opts for potential looping (we also use it to jump back to opts start after parsing opt(s))
          a = i;
          clearWord(search);
          mode = P_OPTS;
        }
        if (ch == '\n') { 
          clearWord(search);
          continue;
        }
        break;
      case P_NAME:
        if (ch == '\n') {
          renderName(word->string);
          clearWord(word);
          mode = P_SEARCH;
        } else {
          addchar(word, ch);
        }
        break;
      case P_DESC:
        if (ch == '\n') {
          if (isWord(search, ".dend")) {
            mode = P_SEARCH;
            // TODO: set description
            renderDesc(word->string);
            clearWord(word);
          } else {
            if (ch != '\t') addchar(line, ch);
            catWords(word, line);
          }
          clearWord(line);
          clearWord(search);
        } else if (ch != '\t') {
          addchar(line, ch);
        }
        if (ch != ' ' && ch != '\t' && ch != '\n') {
          addchar(search, ch);
        }
        break;
      case P_OPTS:
        if (ch == ' ' || ch == '\t') continue;
        addchar(search, ch);
        // if we run into opt, loop until we find .tpo (if EOF is found, report error and bail)
        // continue until .stpo is found, and when that is found, record stop of opts and present player with options for each opt
        if (isWord(search, ".opt")) {
          // when we find an opt, record its position and name for P_OPTS_RUN
          opt_depth = 0;
          clearWord(search);
          p_i++; // opt #
          mode = P_OPT;
        } else if (isWord(search, ".loop")) {
          // loop this set of options
          loop_i = 1;
          clearWord(search);
        } else if (isWord(search, ".lend")) {
          //printf("found end of list, running\n");
          mode = P_OPTS_RUN;
          //b = i;
          clearWord(search);
        }
        if (ch == '\n') { 
          clearWord(search);
          continue;
        }
        //mode = P_SEARCH;
        break;
      case P_OPTS_RUN:
        //printf("running %d opts for range of %d to %d :)\n", p_i, a, b);
        // present player with a char* list of options, with integer selections
        // when player inputs a number, call parse_r(buffer, loc of opt)
        // when parse_r returns, and if loop is true, set i to recorded opts start
        // else if loop is false, or parse_r returns 1 while loop is true, set i to recorded opts stop
        // int choice = handleOptions(options);
        //while(1) {
          /*while((opt = readWordList(options)) != NULL) {
            printf("%d. %s\n", options->pos, opt->string);
          }*/
          // TEMP
          //choice = atoi(gets(&input))-1;
        {}
          int choice = handleOptions(options);
          int ret = parse_r(buffer, options_pos->pos[choice].begin, options_pos->pos[choice].end);
          // this is so wonderful :) (NOT.)
          if (ret == 10) { // exit
            freeWord(meta);
            freeWord(cond);
            freeWord(search);
            freeWord(word);
            freeWord(line);
            freeWordList(options);
            freePosList(options_pos);
            return 10;
          } else if (ret == 20) { // go <map>
            freeWord(meta);
            freeWord(cond);
            freeWord(search);
            freeWord(word);
            freeWord(line);
            freeWordList(options);
            freePosList(options_pos);
            return 20;
          } else if (ret == 30) { // die
            freeWord(meta);
            freeWord(cond);
            freeWord(search);
            freeWord(word);
            freeWord(line);
            freeWordList(options);
            freePosList(options_pos);
            return 30;
          } else if (ret == 40) { // win
            freeWord(meta);
            freeWord(cond);
            freeWord(search);
            freeWord(word);
            freeWord(line);
            freeWordList(options);
            freePosList(options_pos);
            return 40;
          } else if ((ret == 1 && loop_i == 1) || loop_i == 0) {
            //break;
            mode = P_SEARCH;
          } else {
            i = a;
            mode = P_OPTS;
          }
          // END TEMP
        //}
        clearWordList(options);
        clearPosList(options_pos);
        p_i = 0;
        break;
      // kinda like NAME
      case P_OPT:
        if (ch == '\n') {
          // TODO: set opts[p_i] to dis werd
          Word *opt_name = newWord(32);
          catWords(opt_name, word);
          //printf("opt %d start @ %d: \"%s\"\n", p_i, i, opt_name->string);
          clearWord(word);
          addWordList(options, opt_name);
          addPos(options_pos, i, i);
          // add position to Position list, we will manually set end later
          mode = P_OPT_CLOSE;
        } else {
          addchar(word, ch);
        }
        break;
      // kinda like DESC
      case P_OPT_CLOSE:
        // switch to the noms until .tpo at current depth is found
        if (ch == ' ' || ch == '\t') continue;
        addchar(search, ch);
        if (isWord(search, ".opt")) {
          opt_depth++;
        } else if (isWord(search, ".break")) {
          options_pos->pos[p_i-1].brk = 1;
        } else if (isWord(search, ".tpo")) {
          if (opt_depth <= 0) {
            //printf("closing out of opt\n");
            // set this option's position end
            options_pos->pos[p_i-1].end = i;
            // return to P_OPTS for opt parsing
            mode = P_OPTS;
          } else {
            opt_depth--;
          }
        }
        if (ch == '\n') { 
          clearWord(search);
        }
        break;
    }
  }
  freeWord(meta);
  freeWord(cond);
  freeWord(search);
  freeWord(word);
  freeWord(line);
  freeWordList(options);
  freePosList(options_pos);
  return 0;
}
// temp main
/*int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("Usage: %s file.vad\n", argv[0]);
    return 1;
  }
  variables = newVarTable(128);
  //printf("reading in %s\n", argv[1]);
  char *buf = NULL;
  int buf_size = readfile(&buf, argv[1]);
  //printf("read %d bytes, content is:\n", buf_size);
  //printf("%s\n", buf);
  parse_r(buf, 0, buf_size);
  free(buf);
  freeVarTable(variables);
  return 0;
}*/
