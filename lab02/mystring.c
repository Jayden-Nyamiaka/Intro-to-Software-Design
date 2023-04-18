#include "mystring.h"
#include <stdlib.h>
#include <string.h>

char **resizeArray(char **arr, int size);
char *resizeWord(char *word, int length);

strarray *strsplit(const char *str) {
  size_t len = strlen(str);
  char **data = malloc(len * sizeof(char *));
  int size = 0;
  int in_word = 0;

  char *word;
  int j;
  for (size_t i = 0; i < len; i++) {
    if (str[i] == ' ') {
      if (in_word) {
        word = resizeWord(word, j);
        data[size] = word;
        size++;
      }
      in_word = 0;
    } else {
      if (!in_word) {
        word = malloc((len - i + 1) * sizeof(char));
        j = 0;
        in_word = 1;
      }
      word[j] = str[i];
      j++;
    }
  }
  if (in_word) {
    word = resizeWord(word, j);
    data[size] = word;
    size++;
  }
  data = resizeArray(data, size);
  strarray *split = malloc(sizeof(strarray));
  split->data = data;
  split->length = size;
  return split;
}

void strarray_free(strarray *s) {
  for (int i = 0; i < s->length; i++) {
    free(s->data[i]);
  }
  free(s->data);
  free(s);
}

char *resizeWord(char *word, int length) {
  char *out = malloc((length + 1) * sizeof(char));
  for (int i = 0; i < length; i++) {
    out[i] = word[i];
  }
  out[length] = '\0';
  free(word);
  return out;
}

char **resizeArray(char **arr, int size) {
  char **out = malloc(size * sizeof(char *));
  for (int i = 0; i < size; i++) {
    out[i] = arr[i];
  }
  free(arr);
  return out;
}