#include <stdio.h>
#include <stdlib.h>

int checkUsage(int argc, char* first);
int countLines(char* filename);

int main(int argc, char *argv[]) {
    if (checkUsage(argc, argv[0]) == 1) {
        return 1;
    }
    
    if (countLines(argv[1]) == 1) {
        return 1;
    }
    return 0;
}


// returns 0 if valid usage, 1 if invalid
int checkUsage(int argc, char* first) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", first);
        return 1;
    }
    return 0;
}


// prints the file info given filename
// returns 1 if file does not exists, 0 otherwise (working)
int countLines(char* filename) {
    // checks file exist
    FILE *file = fopen(filename, "r");
    if (file == NULL){
        printf("wc: %s: No such file or directory\n", filename);
        return 1;
    }

    int c, count = 0;
    while (!feof(file)){
        c = getc(file);
        if (c == '\n'){
            count++;
        }
    }

    printf("%d %s\n", count, filename);
    fclose(file);
    return 0;
}