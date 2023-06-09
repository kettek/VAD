#ifndef FIFO_H
#define FIFO_H
#include <stdio.h>

/*
struct Dir, struct DirEntry, openDir, readDir, and closeDir act as a cross-platform wrapper to ordered directory listing. Note that openDir() reads in ALL files and readDir() simply provides the next one in the DirEntry linked list. This is unlike opendir()/readdir(), which provides a live structure (or so I think).
*/
#define SORT_DESCEND 0
#define SORT_ASCEND 1
#define F_UKN 0
#define F_REG 1
#define F_DIR 2
#define F_LNK 3
struct Dir {
  char *dirname;
  int order;
  struct DirEntry *start;
  struct DirEntry *current;
};
struct Dir *openDir(const char *path, int order);
int closeDir(struct Dir *dir);
struct DirEntry {
  char *d_name;
  unsigned char d_type;
  struct DirEntry *next; // lazy
};
struct DirEntry *readDir(struct Dir *dir);

int fileExists(const char *filename);
int deleteFile(const char *filename);
int hasExtension(const char *name, const char *extension);

#endif
