#include <stdlib.h>
#include <string.h>

#include "pathlib.h"

bool lib_path_is_path_separator(char ch) {
    return ch  == '\\' || ch == '/';
}

int lib_path_count_path_level(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    int start = 0;
    if (lib_path_is_path_separator(path[start])) {
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
        if (lib_path_is_path_separator(ch)) {
            level++;
        }
        i++;
    }
    return level;
}

// [allocate]
// level starts with '0'
char* lib_path_get_level_path(const char* path, int level) {
    if (path == NULL || path[0] == '\0' || level < 0) {
        return NULL;
    }

    int start = 0;
    if (lib_path_is_path_separator(path[start])) {
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
        if (lib_path_is_path_separator(ch)) {
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

// [allocate]
char** lib_path_split_path(const char* path) {
    if (!path) {
        return NULL;
    }
    int count = lib_path_count_path_level(path);
    if (count == 0) {
        return NULL;
    }
    char** result = (char**) malloc(sizeof(char*) * count + 1);

    // TODO: Optimaze without 'getLevelPath'
    // Temp solution
    for (int i = 0; i < count; i++) {
        result[i] = lib_path_get_level_path(path, i);
    }

    result[count] = NULL;
    return result;
}
