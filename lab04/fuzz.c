#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <ctype.h>
#include "match.h"

const size_t GREP_CMD_LEN = 23;

void check(bool actual, char *pattern, char *text) {
    char *cmd_buffer = malloc((GREP_CMD_LEN + strlen(text) + strlen(pattern)) * sizeof(char));
    strcpy(cmd_buffer, "echo \"");
    strcat(cmd_buffer, text);
    strcat(cmd_buffer, "\" | grep -c \"^");
    strcat(cmd_buffer, pattern);
    strcat(cmd_buffer, "$\"\0");

    char result[1024];
    FILE *grep = popen(cmd_buffer, "r");
    fgets(result, sizeof(result), grep);
    pclose(grep);
    free(cmd_buffer);

    if(actual == true) {
        assert(result[0] == '1');
    } else {
        assert(result[0] == '0');
    }
}

bool is_valid(char *pattern, size_t len) {
    for(int i=0; i<len; i++) {
        if(!isalpha(pattern[i]) && (pattern[i] != '.' && pattern[i] != '*')) return false;
        if(pattern[i] == '*' && pattern[i+1] == '*') return false;
    }
    return true;
}

int LLVMFuzzerTestOneInput(uint8_t *data, size_t size) {
    size_t intsize = size / sizeof(int8_t);
    uint8_t *intdata = (uint8_t *)data;
    if (intsize >= 2) {
        size_t len1 = intdata[0];
        if (size >= sizeof(uint8_t) + len1) {
            size_t len2 = size - sizeof(uint8_t) - len1;
            char *str = ( (char *)intdata ) + 1;
            char *str1 = calloc(len1 + 1, sizeof(char));
            char *str2 = calloc(len2 + 1, sizeof(char));
            strncpy(str1, str, len1);
            strncpy(str2, str + len1, len2);

            if (is_valid(str1, len1) && is_valid(str2, len2))  {
                printf("s1=%s, s2=%s\n", str1, str2);
                bool result = match(str1, str2);
                check(result, str1, str2);
                free(str1);
                free(str2);
                return result;
            }
            free(str1);
            free(str2);
        }
    }
    return 0;
}

