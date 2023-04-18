#include "match.h"


bool checkCharacter(char *target, char *character) {
    // Empty String Base Case
    if (*target == '\0' && *character == '\0') {
        return true;
    }

    if (*target == '.') {
        // Feature 2 (.)
        if (* (target + 1) != '*') {
            if (*character == '\0') {
                return false;
            }
            return checkCharacter(target + 1, character + 1);
        }
        // Feature 4 (.*)
        while (*character != '\0') {
            if (checkCharacter(target + 2, character)) {
                return true;
            }
            character++;
        }
        return checkCharacter(target + 2, character);
    } else {
        // Feature 1 (a-z, A-Z)
        if (* (target + 1) != '*') {
            if (*target != *character) {
                return false;
            }
            return checkCharacter(target + 1, character + 1);
        }
        // Feature 3 (a*)
        while (*target == *character) {
            if (checkCharacter(target + 2, character)) {
                return true;
            }
            character++;
        }
        return checkCharacter(target + 2, character);
    }
    return false;
}


bool match(char *target, char *text) {
    // Edge case where first character is '*' acting as letter
    if (*target == '*') {
        if (*text == '*') {
            return checkCharacter(target + 1, text + 1);
        }
        return false;
    }
    return checkCharacter(target, text);
}