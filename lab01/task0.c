#include <stdio.h>
#include <string.h>

void string_print(char *s) {
    for (int i = 0; s[i] != '\0';i++) {
        printf("%02d: %c\n", i, s[i]);
    }
}

int main(int argc, char *argv[]) {
    string_print("Adam");
}