#include "VarTable.h"
#include "Word.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>


VarTable *newVarTable(int hash_size) {
  VarTable *table = malloc(sizeof(VarTable));
  table->hash_size = hash_size;
  table->item = malloc(hash_size * (sizeof(Var*)));
  int i;
  for (i = 0; i < hash_size; i++) {
    table->item[i] = NULL;
  }
  return table;
}
int freeVarTable(VarTable *table) {
  int i;
  for (i = 0; i < table->hash_size; i++) {
    Var *var = table->item[i];
    while (var != NULL) {
      Var *next_var = var->next;
      freeVar(var);
      var = next_var;
    }
  }
  free(table->item);
  free(table);
  return 0;
}
int setVar(VarTable *table, const char *string, int value) {
  int hash = getHash(table->hash_size, string);
  Var *var = table->item[hash];
  Var *last_var = NULL;
  while(var != NULL) {
    if (strcmp(var->name, string) == 0) {
      var->value = value;
      return 0;
    } else {
      last_var = var;
      var = var->next;
    }
  }
  // if we got here, the var does not exist!
  Var *new_var = newVar(string, value);
  if (last_var == NULL) {
    table->item[hash] = new_var;
  } else {
    last_var->next = new_var;
  }
  return 1;
}

int interpVar(VarTable *table, const char *string) {
  char *variable = NULL;
  int str_len = strlen(string);
  int val = 0;
  int i;
  int mode = 0;
  for (i = 0; i < str_len+1; i++) {
    if (mode == 0) {
      if (string[i] == ' ') {
        variable = malloc(i+1);
        memcpy(variable, string, i);
        variable[i] = '\0';
        mode = 1;
      } else if (string[i] == '\n' || string[i] == '\0') {
        variable = malloc(i+1);
        memcpy(variable, string, i);
        variable[i] = '\0';
        val = 1;
        mode = 2;
      }
    } else if (mode == 1) {
      if (string[i] == '\n' || string[i] == '\0') {
        int temp_size = str_len-i;
        char *temp = malloc(temp_size+1);
        memcpy(temp, string+i, temp_size+1);
        val = atoi(temp);
        free(temp);
        mode = 2;
      }
    }
  }
  //printf("creating variable %s->%d\n", variable, val);
  setVar(table, variable, val);
  free(variable);
  return val;
}

Var *newVar(const char *string, int value) {
  Var *var = malloc(sizeof(Var));
  var->value = value;
  var->next = NULL;
  int str_len = strlen(string);
  var->name = malloc(str_len+1);
  memcpy(var->name, string, str_len+1);
  return var;
}
int freeVar(Var *var) {
  free(var->name);
  free(var);
  return 0;
}
int checkVar(VarTable *table, const char *string) {
  WordList *vars;
  Word *word;
  int len = strlen(string);
  int i;
  int ret = 0;
  // This is all really bad, but gotta get it vaguely working :S
  int found = 0;
  for (i = 0; i < len+1; i++) {
    switch(string[i]) {
      case '&':
        vars = newWordList(8);
        explodeString(vars, '&', string);
        int bleh = 0;
        while ((word = readWordList(vars)) != NULL) {
          bleh++;
          ret += getVar(table, word->string);
        }
        if (ret != bleh) {
          ret = 0;
        } else {
          ret = 1;
        }
        freeWordList(vars);
        i = len;
        found = 1;
        break;
      case '=':
        vars = newWordList(8);
        explodeString(vars, '=', string);
        if (getVar(table, vars->item[0]->string) == atoi(vars->item[1]->string)) {
          ret = 1;
        } else {
          ret = 0;
        }
        freeWordList(vars);
        i = len;
        found = 1;
        break;
    }
  }
  if (found == 0) {
    ret = getVar(table, string);
  }
  return ret;
}

int getVar(VarTable *table, const char *string) {
  int hash = getHash(table->hash_size, string);
  if (table->item[hash] == NULL) {
    return 0;
  }
  Var *var = table->item[hash];
  while(var != NULL) {
    if (strcmp(var->name, string) == 0) {
      return var->value;
    } else {
      var = var->next;
    }
  }
  return 0;
}

int getHash(int size, const char *string) {
  int hash = 0, i;
  int str_len = strlen(string);
  for (i = 0; hash < INT_MAX && i < str_len; i++) {
    hash += string[i];
  }
  return hash % size;
}
