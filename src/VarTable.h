#ifndef VARTABLE_H
#define VARTABLE_H
typedef struct VarTable {
  int hash_size;
  struct Var **item;
} VarTable;
VarTable *newVarTable(int hash_size);
int freeVarTable(VarTable *table);
int setVar(VarTable *table, const char *string, int value);
int checkVar(VarTable *table, const char *string);
int getVar(VarTable *table, const char *string);

int interpVar(VarTable *table, const char *string);


typedef struct Var {
  char *name;
  int value;
  struct Var *next;
} Var;
Var *newVar(const char *string, int value);
int freeVar(Var *var);

int getHash(int hash_size, const char *string);
#endif
