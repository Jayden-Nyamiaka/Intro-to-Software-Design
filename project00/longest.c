#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int checkUsage(int argc, char* first);
int findLongestWord(char* filename);

int main(int argc, char *argv[]) {
    if (checkUsage(argc, argv[0]) == 1) {
        return 1;
    }
    
    if (findLongestWord(argv[1]) == 1) {
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
int findLongestWord(char* filename) {
    // checks file exist
    FILE *file = fopen(filename, "r");
    if (file == NULL){
        printf("longest: %s: No such file or directory\n", filename);
        return 1;
    }

    int c, count = 0, max = 0;
    char* current = (char *)malloc(101 * sizeof(char));
    char* longest = (char *)malloc(101 * sizeof(char));
    while (1){
        c = getc(file);
        if (c == ' ' || c == '\n' || feof(file)) {
            if (count > max) {
                current[count] = '\0';
                max = count;
                strcpy(longest, current);
            }
            count = 0;
            if (feof(file)) {
                break;
            }
        } else {
            current[count] = c;
            count++;
        }
    }
            
    // empty file print nothing
    if (max != 0) {
        printf("%s\n", longest);
    }

    free(current);
    free(longest);
    fclose(file);
    return 0;
}