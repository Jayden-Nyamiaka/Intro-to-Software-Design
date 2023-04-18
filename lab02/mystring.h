#ifndef MYSTRING_H
#define MYSTRING_H

typedef struct strarray {
  char **data;
  int length;
} strarray;

strarray *strsplit(const char *str);

void strarray_free(strarray *s);

#endif
