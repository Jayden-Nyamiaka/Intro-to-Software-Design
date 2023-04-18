#include "mystring.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const int MAX_INPUT_SIZE = 10000;

int main(int argc, char *argv[]) {
  /*
  char *word = malloc(10 * sizeof(char));
  word[0] = 'l';
  word[1] = 'a';
  word[2] = 'a';
  word[3] = 'l';
  word[4] = 'l';
  word[5] = ' ';
  word[6] = ' ';
  word[7] = 'a';
  word[8] = 'a';
  word[9] = '\0';
  strarray *s = strsplit(word);
  for (int i = 0; i < s->length; i++) {
      printf("%d: %s\n", i, s->data[i]);
  }
  free(word);
  strarray_free(s);
  */

  assert(argc == 2 && "usage: mystringtest file");
  char *buf = malloc(MAX_INPUT_SIZE);
  FILE *in = fopen(argv[1], "r");
  assert(in && "file not found");
  buf[fread(buf, sizeof(char), MAX_INPUT_SIZE, in) - 1] = '\0';
  fclose(in);
  strarray *result = strsplit(buf);
  for (int i = 0; i < result->length; i++) {
    printf("%d: \"%s\"\n", i, result->data[i]);
  }
  strarray_free(result);
  free(buf);
  return 0;
}
