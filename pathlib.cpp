#include <stdlib.h>
#include <string.h>

#include "pathlib.h"

bool isPatchChar(char ch) {
    return ch  == '\\' || ch == '/';
}

int countPathLevel(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return 0;
    }
    int start = 0;
    if (isPatchChar(path[start])) {
        start++;
    }
    if (path[start] == '\0') {
        return 0;
    }
    char ch;
    int i = start;
    int level = 1;
    int end = -1;

    // find start level
    while ((ch = path[i]) != '\0') {
        if (isPatchChar(ch)) {
            level++;
        }
        i++;
    }
    return level;
}

char* getLevelPath(const char* path, int level) {
    if (path == NULL || path[0] == '\0' || level < 0) {
        return NULL;
    }
    int start = 0;
    if (isPatchChar(path[start])) {
        start++;
    }
    if (path[start] == '\0') {
        return NULL;
    }
    char ch;
    int i = start;
    int curr_level = -1;
    int end = -1;

    // find start level
    while ((ch = path[i]) != '\0') {
        if (isPatchChar(ch)) {
            curr_level++;
            if (level - curr_level == 1) {
                start = i + 1;
            }
            if (curr_level == level) {
                end = i;
                break;
            }            
        }
        i++;
    }

    //printf("curr level: %d\n", curr_level);
    //printf("start path: %d\n", start);

    if (curr_level < 0) {
        // No level in pattern
        end = i;
    } else if (curr_level < level) {
        if (level - curr_level == 1) {
            end = i;
        } else {
            // Incorrect deep level in pattern
            return NULL;
        }
    }

    //printf("end path  : %d\n", end);

    if (start == end) {
        end++;
    }    

    int len = end - start;
    char* p = (char*) malloc(len + 1);
    strncpy(p, path + start, len);
    p[len] = '\0';
    return p;

}

////

bool isWildcardChar(char ch) {
    return (ch == '*' || ch == '?' || ch == '[' || ch == ']');
}

int isWildcard(const char* pattern) {
    return getWildcardIndex(pattern) != -1;
}

int getWildcardIndex(const char* pattern) {
    if (pattern == NULL) {
        return -1;
    }
    int len = strlen(pattern);
    if (len == 0) {
        return -1;
    }
    for (int i = 0; i < len; i++) {
        if (isWildcardChar(pattern[i])) {
            return i;
        }
    }
    return -1;
}

// ../a/name*.text - > 4 -> '/'
int getWildcardPathIndex(const char* pattern, const char* fileName) {
    if (pattern == NULL || fileName == NULL) {
        return -1;
    }
    int wildcardIndex = getWildcardIndex(pattern);
    if (wildcardIndex < 0) {
        return -1;
    }
    return getWildcardPathIndex(wildcardIndex, fileName);
}

int getWildcardPathIndex(int wildcardIndex, const char* fileName) {
    if (fileName == NULL) {
        return -1;
    }
    int i = wildcardIndex;
    // go from end to start
    bool found = false;
    while (i >= 0) {
        if (isPatchChar(fileName[i])) {
            found = true;
            break;
        }
        i--;
    }
    return found ? i : -1;
}

