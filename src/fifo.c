#include "fifo.h"
#include <stdio.h> // remove()
#include <stdint.h> // uint32_t
#if _WIN32 | _WIN64
#include <Windows.h>
#include <Strsafe.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <dirent.h> // scandir
#include <sys/stat.h> // stat
#include <unistd.h> // access()
#include "string.h"
#include <errno.h>
struct Dir *openDir(const char *path, int order) {
  if (path == NULL) return NULL;
  struct Dir *dir = malloc(sizeof(struct Dir));
  if (dir == NULL) {
    printf("malloc failed\n");
    return NULL;
  }
  dir->current = NULL;
  dir->start = NULL;
  dir->order = order;
  int size = strlen(path)+1;
  dir->dirname = malloc(size);
  memcpy(dir->dirname, path, size);
  #if _WIN32 | _WIN64
  WIN32_FIND_DATA ffd;
  HANDLE h_find = INVALID_HANDLE_VALUE;

  char crappy_windows_dir[MAX_PATH];
  StringCbCopyN(crappy_windows_dir, MAX_PATH, dir->dirname, size);
  StringCbCatN(crappy_windows_dir, MAX_PATH, "*", 2);

  h_find = FindFirstFile(crappy_windows_dir, &ffd);
  if (h_find == INVALID_HANDLE_VALUE) {
    // some sort of rubbish windows error
  }
  struct DirEntry *last_entry = NULL;
  do {
    struct DirEntry *entry = malloc(sizeof(struct DirEntry));
    // copy over filename
    int new_size = strlen(ffd.cFileName)+1;
    entry->d_name = malloc(new_size);
    memcpy(entry->d_name, ffd.cFileName, new_size);
    // NULL it!
    entry->next = NULL;
    if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      entry->d_type = F_DIR;
    } else {
      entry->d_type = F_REG;
    }
    // initial linked list if not yet done
    if (dir->start == NULL) {
      dir->start = entry;
      dir->current = entry;
    }
    // assign this entry to last_entry's next if possible
    if (last_entry != NULL) {
      last_entry->next = entry;
    }
    // set this entry to last entry
    last_entry = entry;
  } while (FindNextFile(h_find, &ffd));
  FindClose(h_find);
  #else
  struct dirent **files;
  int n;
  n = scandir(dir->dirname, &files, NULL, alphasort);
  if (n < 0) {
    perror("openDir");
    return NULL;
  } else {
    struct DirEntry *last_entry = NULL;
    int i;
    for(i = 0; i < n; i++) {
      struct DirEntry *entry = malloc(sizeof(struct DirEntry));
      // copy over filename
      entry->d_name = malloc(1);
      entry->d_name = copyString(entry->d_name, files[i]->d_name);
      // NULL it!
      entry->next = NULL;
      // build full path name
      int dir_length = strlen(dir->dirname);
      int file_length = strlen(files[i]->d_name);
      char filepath[dir_length+file_length+2];
      memcpy(filepath, dir->dirname, dir_length);
      filepath[dir_length] = '/';
      memcpy(filepath+dir_length+1, files[i]->d_name, file_length+1);
      // get file info
      struct stat file_stat;
      stat(filepath, &file_stat);
      switch(file_stat.st_mode & S_IFMT) {
        case S_IFDIR:
          entry->d_type = F_DIR;
          break;
        case S_IFREG:
          entry->d_type = F_REG;
          break;
        case S_IFLNK:
          entry->d_type = F_LNK;
          break;
        default:
          entry->d_type = F_UKN;
          break;
      }
      // initial linked list if not yet done
      if (dir->start == NULL) {
        dir->start = entry;
        dir->current = entry;
      }
      // assign this entry to last_entry's next if possible
      if (last_entry != NULL) {
        last_entry->next = entry;
      }
      // set this entry to last entry
      last_entry = entry;
      free(files[i]);
    }
    free(files);
  }
  #endif
  return dir;
}
int closeDir(struct Dir *dir) {
  if (dir == NULL) return 1;
  struct DirEntry *entry = dir->start;
  while(entry != NULL) {
    struct DirEntry *last_entry = entry;
    entry = entry->next;
    free(last_entry->d_name);
    free(last_entry);
  }
  free(dir->dirname);
  free(dir);
  return 0;
}
struct DirEntry *readDir(struct Dir *dir) {
  if (dir == NULL) return NULL;
  if (dir->current == NULL) return NULL;
  struct DirEntry *entry = dir->current;
  dir->current = entry->next;
  return entry;
}

int fileExists(const char *filename) {
  if (access(filename, F_OK) == 0) {
    return 1;
  }
  return 0;
}
int deleteFile(const char *filename) {
  if (filename == NULL) return 1;
  if (!fileExists(filename)) {
    return 2;
  }

  return remove(filename);
}

int hasExtension(const char *name, const char *extension) {
  if (name == NULL || extension == NULL) return 0;
  char *exten;
  if ((exten = strrchr(name, '.')) == NULL) {
    return 0;
  }
  if (strcmp(exten, extension) != 0) {
    return 0;
  }
  return 1;
}
