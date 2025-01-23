#include <stdlib.h>
#include <string.h>

#include "pathlib.h"

bool lib_path_is_separator(char ch) {
    // cross-platform path separator: '/', '\'
    return ch  == '\\' || ch == '/';
}

int lib_path_count_level(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    int start = 0;
    if (lib_path_is_separator(path[start])) {
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
        if (lib_path_is_separator(ch)) {
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
    if (lib_path_is_separator(path[start])) {
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
        if (lib_path_is_separator(ch)) {
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
char** lib_path_split(const char* path) {
    if (!path) {
        return NULL;
    }
    int count = lib_path_count_level(path);
    if (count == 0) {
        return NULL;
    }
    char** result = (char**) malloc(sizeof(char*) * count + 1);

    // TODO: Optimaze without 'get_level_path'
    // Temp solution
    for (int i = 0; i < count; i++) {
        result[i] = lib_path_get_level_path(path, i);
    }

    result[count] = NULL;
    return result;
}

static inline ssize_t _lib_path_rskip_sep(char* path, ssize_t i) {
    //while (i >= 0 && (path[i] == '/' || path[i] == '\\')) {
    while (i >= 0 && lib_path_is_separator(path[i])) {
        i--;
    }
    return i;
} 

static inline ssize_t _lib_path_rskip_char(char* path, ssize_t i) {
    //while (i >= 0 && (path[i] != '/' && path[i] != '\\')) {
    while (i >= 0 && !lib_path_is_separator(path[i])) {
        i--;
    }
    return i;
} 

char* lib_path_base(char* path) {
    if (!path) {
        return NULL;
    }
    size_t len = strlen(path);
    if (len == 0) {
        return path;
    }

    //fprintf(stderr, ">> len   : %lu\n", len);

    // skip slashes and stop at first char or out side
    ssize_t i = len - 1;
    i = _lib_path_rskip_sep(path, i);

    //fprintf(stderr, ">> last  : %ld\n", i);

    if (i == -1) {
        //fprintf(stderr, ">> return ''\n");

        // out side: all chars are slashes, for example
        // '/',  '\\',  '////', '\\\\', '////\\\\' , '\\\\////'

        path[0] = '\0';
        return path;
    }

    // skip chars and stop at first slashe or out side
    ssize_t j = i;
    i = _lib_path_rskip_char(path, i);

    char* base = path;
    
    //fprintf(stderr, ">> first : %ld\n", i);

    if (j > 0 && j < len - 1) {
        //fprintf(stderr, ">> trunc : %ld\n", j);
        base[j + 1] = '\0'; // truncate
    }

    if (i >= 0) {
        //fprintf(stderr, ">> offset: %ld\n", i + 1);
        base = base + i + 1; // offset
    }

    //fprintf(stderr, ">> base  : %s\n", base);

    return base;
}

char* lib_path_parent(char* path) {
    if (!path) {
        return NULL;
    }
    size_t len = strlen(path);
    if (len == 0) {
        return path;
    }

    //fprintf(stderr, ">> len   : %lu\n", len);

    // skip slashes and stop at first char or out side
    ssize_t i = len - 1;
    i = _lib_path_rskip_sep(path, i);

    //fprintf(stderr, ">> last  : %ld\n", i);

    if (i == -1) {
        //fprintf(stderr, ">> return[1] '/'\n");

        // out side: all chars are slashes, for example
        // '/',  '\\',  '////', '\\\\', '////\\\\' , '\\\\////'

        path[0] = '/'; // TODO: What about WIN?
        path[1] = '\0';
        return path;
    }

    // skip chars and stop at first slashe or out side
    ssize_t j = i;
    i = _lib_path_rskip_char(path, i);

    if (i == -1) {
        //fprintf(stderr, ">> return[2] '.'\n");

        // out side: for example
        // 'abc/',  'abc\\',  'abc////', 'abc\\\\', 'abc////\\\\' , 'abc\\\\////'

        path[0] = '.'; // TODO: What about WIN?
        path[1] = '\0';
        return path;
    }

    // skip slashes and stop at first char or out side
    i = _lib_path_rskip_sep(path, i);

    if (i == -1) {
        //fprintf(stderr, ">> return[3] '/'\n");

        // out side: for example
        // '/abc/',  '\\abc\\',  '////abc////', '\\\\abc\\\\', '////\\\\abc////\\\\' , '\\\\////abc\\\\////'

        path[0] = '/'; // TODO: What about WIN?
        path[1] = '\0';
        return path;
    }

    char* parent = path;

    parent[i + 1] = '\0'; // truncate    
    //fprintf(stderr, ">> parent  : %s\n", parent);

    return parent;
}
