#include <stdlib.h>
#include <string.h>
#include <stdio.h>


char *string_reverse(char *s) {
    int len = strlen(s);
    char *rev = (char*)malloc((len + 1) * sizeof(char));
    for (int i = 0; i < len; i++) {
        rev[i] = s[len - 1 - i];
    }
    rev[len] = '\0';
    return rev;
}

int main(int argc, char *argv[]) {
    char *rev1 = string_reverse("aaaaaaaaaa");
    printf("%s\n", rev1);
    free(rev1);
    char *rev2 = string_reverse("bbbbbbbbb");
    printf("%s\n", rev2);
    char *rev3 = string_reverse("represent");
    printf("%s\n", rev3);
    free(rev3);
}