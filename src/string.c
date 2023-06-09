#include <string.h>
#include <stdlib.h>
#include <stdarg.h> // va stuff
#include <stdio.h> // vsnprintf
#include "string.h"

/*
This function resizes dst to strlen(src)+1 and copies src to dst. If dst ptr is NULL, memory is allocated for dst enough to fit src and src is copied over.
*/
char *copyString(char *dst, const char *src) {
  if (dst == NULL) {
    dst = malloc(1);
    dst[0] = '\0';
  }
  if (src == NULL) return dst;
  int size = strlen(src)+1;
  free(dst); // free old memory
  dst = malloc(size);
  memcpy(dst, src, size);
  return dst;
}

// meow
char *catString(char *dst, const char *src) {
  if (dst == NULL) {
    dst = malloc(1);
    dst[0] = '\0';
  }
  if (src == NULL) return dst;
  int dst_len = strlen(dst);
  int src_len = strlen(src);
  free(dst);
  dst = malloc(dst_len+src_len+1);
  memcpy(dst+dst_len, src, src_len+1);
  return dst;
}
// amorpheous meow
char *catStringF(char *dst, const char *string, ...) {
  if (string == NULL) return NULL;
  if (dst == NULL) {
    dst = malloc(1);
    dst[0] = '\0';
  }
  char buf[128];
  char *buffer;
  int overflow;
  va_list args;
    
  buffer = buf;
    
  va_start(args, string);
  overflow = vsnprintf(buffer, 128, string, args);
  va_end(args);
  // did we overflow?
  if (overflow >= 128) {
    buffer = (char*)malloc(overflow+1);
    va_start(args, string);
    overflow = vsnprintf(buffer, overflow+1, string, args);
    va_end(args);
  }

  dst = catString(dst, buffer);

  if (buffer != buf) {
    free(buffer);
  }
  return dst;
}

char *setStringF(char *dst, const char *string, ...) {
  if (dst == NULL) {
    dst = malloc(1);
  }

  char buf[128];
  char *buffer;
  int overflow;
  va_list args;
    
  buffer = buf;
    
  va_start(args, string);
  overflow = vsnprintf(buffer, 128, string, args);
  va_end(args);
  // did we overflow?
  if (overflow >= 128) {
    buffer = (char*)malloc(overflow+1);
    va_start(args, string);
    overflow = vsnprintf(buffer, overflow+1, string, args);
    va_end(args);
  }

  dst = realloc(dst, overflow+1);
  memcpy(dst, buffer, overflow+1);

  if (buffer != buf) {
    free(buffer);
  }
  return dst;
}
