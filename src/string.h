#ifndef STRING_H
#define STRING_H
char *copyString(char *dst, const char *src);
char *catString(char *dst, const char *src);
char *catStringF(char *dst, const char *string, ...);
char *setStringF(char *dst, const char *string, ...);
#endif
