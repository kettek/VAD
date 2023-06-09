#include "Word.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Word *newWord(int size) {
  Word *word = malloc(sizeof(Word));
  word->chunk = size; // resize amount
  word->size = size;
  word->string = malloc(size*sizeof(char));
  memset(word->string, 0, size);
  word->pos = 0;
  return word;
}
int freeWord(Word *word) {
  free(word->string);
  free(word);
  return 0;
}
int clearWord(Word *word) {
  if (( word->string = realloc(word->string, word->chunk*sizeof(char))) == NULL) {
    printf("clearWord: realloc failed\n");
    return 1;
  }
  memset(word->string, 0, word->chunk*sizeof(char));
  word->pos = 0;
  word->size = word->chunk;
  return 0;
}
int addchar(Word *word, char ch) {
  if (word->pos+1 > word->size) {
    if ((word->string = realloc(word->string, word->size+word->chunk)) == NULL) {
      printf("addChar: realloc failed\n");
      return 1;
    }
    memset(word->string+word->size, 0, word->chunk);
    word->size += word->chunk;
  }
  word->string[word->pos++] = ch;
  return 0;
}
int isWord(Word *word, const char *string) {
  if (strcmp(word->string, string) == 0) return 1;
  return 0;
}
int catWords(Word *dst, const Word *src) {
  int new_size = dst->size + src->size;
  int new_pos = dst->pos + src->pos;
  dst->string = realloc(dst->string, new_size);
  memcpy(dst->string+dst->pos, src->string, src->pos);
  memset(dst->string+new_pos, 0, new_size-new_pos);
  dst->size = new_size;
  dst->pos = new_pos;
  return 0;
}
int setWord(Word *dst, const char *string) {
  int len = strlen(string);
  int new_size = dst->size + len+1;
  int new_pos = dst->pos + len;
  dst->string = realloc(dst->string, new_size);
  memcpy(dst->string+dst->pos, string, len+1);
  dst->size = new_size;
  dst->pos = new_pos;
 return 0;
}

// WordList ================================
WordList *newWordList(int size) {
  WordList *list = malloc(sizeof(WordList));
  list->chunk = size;
  list->size = size;
  list->count = 0;
  list->pos = 0;
  list->item = malloc(sizeof(Word*)*size);
  int i;
  for (i = 0; i < size; i++) {
    list->item[i] = NULL;
  }
  return list;
}
int freeWordList(WordList *list) {
  int i;
  for (i = 0; i < list->size; i++) {
    if (list->item[i] != NULL) {
      freeWord(list->item[i]);
    }
  }
  free(list->item);
  free(list);
  return 0;
}
int addWordList(WordList *list, Word *item) {
  // FIXME: memory leak here or something
  if (list->count+1 > list->size) {
    //printf("resizing from %d to %d\n", list->size, list->size+list->chunk);
    if ((list->item = realloc(list->item, list->size+list->chunk)) == NULL) {
      printf("addWordList: realloc failed\n");
      return 1;
    }
    int i;
    for (i = list->count+1; i < list->size+list->chunk; i++) {
      list->item[i] = NULL;
    }
    list->size += list->chunk;
  }
  list->item[list->count++] = item;
  return 0;
}
int clearWordList(WordList *list) {
  int i;
  for (i = 0; i < list->size; i++) {
    if (list->item[i] != NULL) {
      freeWord(list->item[i]);
    }
  }
  // ???
  if (list->item != NULL) {
    free(list->item);
    list->item = NULL;
  }
  if ((list->item = realloc(list->item, list->chunk*sizeof(void*))) == NULL) {
    printf("clearWordList: realloc failed\n");
    return 1;
  }
  for (i = 0; i < list->chunk; i++) {
    list->item[i] = NULL;
  }
  list->pos = 0;
  list->count = 0;
  list->size = list->chunk;
  return 0;
}
Word *readWordList(WordList *list) {
  if (list->pos >= list->count) {
    list->pos = 0;
    return NULL;
  }
  return (list->item[list->pos++]);
}

int explodeString(WordList *list, char ch, const char *string) {
  int i;
  int len = strlen(string);
  int left = 0;
  for (i = 0; i <= len; i++) {
    if (string[i] == ch || string[i] == '\0') {
      // get left-side of string
      char *temp = malloc(i-left+1);
      memcpy(temp, string+left, i-left);
      temp[i-left] = '\0';
      //printf("temp: %s\n", temp);
      Word *var = newWord(0);
      setWord(var, temp);
      addWordList(list, var);
      free(temp);
      left = i+1;
    }
  }
  return 0;
}
