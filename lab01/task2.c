#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char **string_blanks(char *s) {
    int len = strlen(s);
    char **arr = (char **)malloc(len * sizeof(char*));
    for (int i = 0; i < len; i++) {
        arr[i] = (char *)malloc((len + 1) * sizeof(char));
        for (int j = 0; j < len; j++) {
            if (i == j) {
                arr[i][j] = '_';
            } else {
                arr[i][j] = s[j];
            }
        }
        arr[i][len] = '\0';
    }
    return arr;
}

int main(int argc, char *argv[]) {
    char **blanks = string_blanks("Adam");
    for (int i = 0; i < strlen("Adam"); i++) {
        printf("%s ", blanks[i]);
        free(blanks[i]);
    }
    printf("\n");
    free(blanks);
}