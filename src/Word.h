#ifndef WORD_H
#define WORD_H
#include <stdlib.h>
typedef struct Word {
  size_t size; // size
  size_t pos; // current pos
  size_t chunk; // amount to realloc string by if added data would push pos > size
  char *string; // data
} Word;
// newWord - _size_ defines both the starting size as well as the amount to resize if the data to be added is greater than current size
Word *newWord(int size);
int freeWord(Word *word);
int addchar(Word *word, char ch);
int clearWord(Word *word);
int catWords(Word *dst, const Word *src);
int isWord(Word *word, const char *string);
int setWord(Word *dst, const char *string);

typedef struct WordList {
  size_t size;
  size_t chunk;
  int pos; // position for iterator
  int count;
  Word **item;
} WordList;
WordList *newWordList(int size);
int freeWordList(WordList *list);
int addWordList(WordList *list, Word *item);
int clearWordList(WordList *list);
Word *readWordList(WordList *list);

int explodeString(WordList *list, char ch, const char *string);

#endif
