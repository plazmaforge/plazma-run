#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include "fslib_win.h"
#else
#include "fslib_nix.h"
#endif

#include "fslib.h"

/* C Style */

// [allocate]
char* get_file_path(const char* dir_name, const char* file_name) {
    if (!dir_name && !file_name) {
        return NULL;
    }
    if (!dir_name) {
        return strdup(file_name);
    }

    // TODO: Ups...May be NULL: No file name
    if (!file_name) {
        return strdup(dir_name);
    }

    int len1 = strlen(dir_name);
    if (len1 == 0) {
        return strdup(file_name);
    }

    int len2 = strlen(file_name);
    if (len2 == 0) {
        return strdup(dir_name);
    }

    int sep_len = 0;
    if (isPatchChar(dir_name[len1 - 1])) {
        sep_len++;
    }
    if (isPatchChar(file_name[0])) {
        sep_len++;
    }
    if (sep_len == 2) {
        sep_len = -1; // erase 1 position
    } else if (sep_len == 1) {
        sep_len = 0;  // nothing
    } else {
        sep_len = 1;  // add 1 position between dir and file
    }

    int len = len1 + sep_len + len2;

    char* path = (char*) malloc(len + 1);
    strcpy(path, dir_name);
    if (sep_len == 1) {
        path[len1] = '/'; // TODO: OS/FS File separator (!)
        path[len1 + 1] = '\0'; // ???: Maybe for next strcat
        //strcat(path, "/");
    }
    // shift fileName if erase 1 position
    strcat(path, sep_len == -1 ? file_name + 1 : file_name);
    path[len] = '\0';

    return path;
}

int match_file(const char* name, const char* pattern) {
    if (name == NULL || pattern == NULL) {
        return 0;
    }

    do {
        switch (*pattern) {
        case '\0':
            /* Only end of string matches NUL */
            return *name == '\0';

        case '/':
        case '\\':
        case ':':
            /* Invalid pattern */
            return 0;

        case '?':
            /* Any character except NUL matches question mark */
            if (*name == '\0')
                return 0;

            /* Consume character and continue scanning */
            name++;
            pattern++;
            break;

        case '*':
            /* Any sequence of characters match asterisk */
            switch (pattern[1]) {
            case '\0':
                /* Trailing asterisk matches anything */
                return 1;

            case '*':
            case '?':
            case '/':
            case '\\':
            case ':':
                /* Invalid pattern */
                return 0;

            default:
                /* Find the next matching character */
                while (*name != pattern[1]) {
                    if (*name == '\0')
                        return 0;
                    name++;
                }

                /* Terminate sequence on trailing match */
                if (match_file(name, pattern + 1))
                    return 1;

                /* No match, continue from next character */
                name++;
            }
            break;

        default:
            /* Only character itself matches */
            if (*pattern != *name)
                return 0;

            /* Character passes */
            name++;
            pattern++;
        }
    } while (1);

    return 0;
}

/* C++ Style - Migration Task */

//[allocate]
char* getFilePath(const char* dirName, const char* fileName) {
    return get_file_path(dirName, fileName);
}

//[allocate]
char* getRealPath(const char* path) {
    return get_real_path(path);
}

std::string getCurrentFindPath() {
    std::string str(get_current_find_path());
    return str;
}

bool isCurrentFindPath(const char* path) {
    return is_current_find_path(path);
}

////

std::vector<std::string> getFiles(const char* dirName) {
    return getFiles(dirName, NULL);
}

std::vector<std::string> getFiles(const char* dirName, const char* pattern) {
    std::vector<std::string> files;
    scandir(dirName, pattern, files);
    return files;
}

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files) {
    scandir(dirName, pattern, files, FS_SCANDIR_FLAT);
}

// Scandir source code
// https://gist.github.com/saghul/8013376

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files, int level) {
    if (dirName == NULL) {
        return;
    }

    int max_depth = level;
    if (max_depth < 0) {
        level = 0;
    }
    int total_level = countPathLevel(pattern); // count path level in pattern
    char* level_pattern = getLevelPath(pattern, level); // [allocate]

    //printf("scandir : %s\n", dirName);
    //printf("total   : %d\n", total_level);
    //printf("level   : %d\n", level);
    //printf("pattern : %s\n", pattern);
    //printf("select  : %s\n", level_pattern);

    scandir_internal(dirName, pattern, files, level, max_depth, total_level, level_pattern);

    free(level_pattern);
}

// [strlib]
int lib_stralen(/*const*/ char** array) {
    if (!array) {
        return 0;
    }
    /*const*/ char* e = NULL;
    /*const*/ char** elements = array;
    int count = 0;
    while ((e = *elements) != NULL) {
        count++;
        elements++;
    }
    return count;
}

// [strlib]
void lib_strafree(char** array) {
    if (!array) {
        return;
    }
    char* e = NULL;
    char** elements = array;
    int count = 0;
    while ((e = *elements) != NULL) {
        free(e);
        elements++;
    }
    free(array);
}

int files_malloc(file_t*** files, size_t size) {
    // NULL-terminate array: +1
    file_t** list = (struct file_t**) malloc(sizeof(struct file_t*) * size + 1); 
    if (!list) {
        return -1;
    }
    list[size] = NULL;
    *files = list;
    return 0;
}

int files_realloc(file_t*** files, size_t size) {
    // NULL-terminate array: +1
    file_t** list = (struct file_t **) realloc((file_t *)*files, (size + 1) * sizeof(struct file_t *));
    list[size] = NULL;
    if (!list) {
        return -1;
    }
    *files = list;
    return 0;
}

int scandir2(const char* dir_name, const char* pattern, file_t*** files, int max_depth) {
    if (!dir_name) {
        return -1;
    }

    char** patterns = split_path(pattern); // split pattern by level
    int pattern_count = lib_stralen(patterns);

    // const char* e = NULL;
    // const char** elements = patterns;
    // while ((e = *elements) != NULL) {
    //     printf(">>pettern       : %s\n", e);
    //     pattern_count++;
    //     elements++;
    // }

    printf(">>pattern_count : %d\n", pattern_count);
    //printf(">>total_level   : %d\n", total_level);
    //printf(">>scandir       : %s\n", dir_name);
    //printf(">>total         : %d\n", total_level);
    //printf(">>level         : %d\n", level);
    //printf(">>pattern       : %s\n", pattern);
    //printf(">>select        : %s\n", level_pattern);

    int file_count = 0;
    int size = 10; // start size

    // NULL-terminate array: +1
    //file_t** list = (struct file_t**) malloc(sizeof(struct file_t*) * size + 1); 
    //list[size] = NULL;
    //*files = list;

    files_malloc(files, size);

    scandir_internal2(dir_name, /*(const char**)*/ patterns, pattern_count, files, &file_count, 0, max_depth);

    lib_strafree(patterns);

    return file_count;
}

