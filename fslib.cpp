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

// [fslib]
file_t* file_new() {
  file_t* file = (file_t*) malloc(sizeof(struct file_t));
  if (!file) {
    return NULL;
  }
  file->name = NULL;
  return file;
}

// [fslib]
void file_free(file_t* file) {
    if (!file) {
        return;
    }
    //free(file->name);
    free(file);
}

// [fslib]
void files_free(file_t** files) {
    if (!files) {
        return;
    }
    file_t* file = NULL;
    file_t** elements = files;
    while ((file = *elements) != NULL) {
        file_free(file);
        elements++;
    }
    free(files);
}

// [fslib]
int files_init(file_t*** files, size_t size) {
    // NULL-terminate array: +1
    file_t** list = (struct file_t**) calloc(size + 1, sizeof(struct file_t*)); 
    if (!list) {
        return -1;
    }
    list[size] = NULL;
    *files = list;
    return 0;
}

// [fslib]
int files_reinit(file_t*** files, size_t size) {
    // NULL-terminate array: +1
    file_t** list = (struct file_t **) realloc( /*(file_t *)*/ *files, (size + 1) * sizeof(struct file_t *));
    list[size] = NULL;
    if (!list) {
        return -1;
    }
    *files = list;
    return 0;
}

int scandir_internal2(const char* dir_name, /*const*/ char** patterns, int pattern_count, file_t*** files, int* file_count, int level, int max_depth) {

    fs_dir_t* dir = open_dir(dir_name);
    if (!dir) {
        return -1;
    }

    fs_dirent_t* file;
    //errno = 0;
    const char* pattern = NULL;
    if (patterns)  {
        pattern = patterns[level];
    } 

    while ((file = read_dir(dir)) != NULL) {

        //printf("FILE: %p\n", file);

        char* file_name = file->name; //file->d_name;

        //printf("try [%d] %s, %s, :: %s\n", level, dir_name, file_name, pattern);
        if (pattern == NULL || match_file_internal(pattern, file_name)) {

            int mode = 0; // 0 - notning, 1 - file, 2 - dir
            if (!is_dir(file)) {
                // We add the file from last pattern level only
                mode = (level == 0 || level == pattern_count - 1) ? 1 : 0;
            } else {
                // Recursive if max_depth != -1
                mode = max_depth >= 0 ? 2 : 0;
            }

            if (mode == 0) {
                continue; // notning
            }

            char* full_name = get_normalize_path(dir_name, file_name);

            //printf("match: full_name    : %s\n", full_name);
    
            if (mode == 1) {

                //printf("try  : add_file\n");
                int index = *file_count; // old file_count
                file_t** list = *files;

                if (list[index] == NULL) { // NULL-terminate array: +1
                    const int inc = 10;	/* increase by this much */
                    int size = index + inc;
                    if (files_reinit(files, size) != 0) {                        
                        free(full_name);
                        files_free(*files);
                        close_dir(dir);
                        return -1;
                    }
                }

                file_t* file_s = file_new(); // (file_t*) malloc(sizeof(struct file_t));
                file_s->name = strdup(full_name);

                //printf("try  : index        : %d\n", *file_count);
                //printf("try  : reserved     : %d\n", *reserved);

                list = *files;
                list[index] = file_s; 
                //*files[*file_count] = file_s; // Doesn't work

                *file_count = *file_count + 1;
                 
                // Shift NULL marker (size + 1): for allocation without fulling NULL
                index = *file_count; // new file_count
                if (list[index] != NULL) {
                    list[index] = NULL;
                }

                //printf("try  : file_count   : %d\n", *file_count);
            } else if (mode == 2) {

               int result = scandir_internal2(full_name, patterns, pattern_count, files, file_count, level + 1, max_depth);
               if (result < 0) {
                // error
               }
            }

            free(full_name);
        }
    }
    //if (errno != 0) {
        // TODO: stderr: error
    //}
    close_dir(dir);
    return 0;
}

int scandir2(const char* dir_name, const char* pattern, file_t*** files, int max_depth) {
    if (!dir_name) {
        return -1;
    }

    char** patterns = split_path(pattern); // split pattern by level
    int pattern_count = lib_stralen(patterns);

    //printf(">>pattern_count : %d\n", pattern_count);
    //printf(">>total_level   : %d\n", total_level);
    //printf(">>scandir       : %s\n", dir_name);
    //printf(">>total         : %d\n", total_level);
    //printf(">>level         : %d\n", level);
    //printf(">>pattern       : %s\n", pattern);
    //printf(">>select        : %s\n", level_pattern);

    int file_count = 0;
    int size = 10; // start size

    files_init(files, size);

    scandir_internal2(dir_name, /*(const char**)*/ patterns, pattern_count, files, &file_count, 0, max_depth);

    lib_strafree(patterns);

    return file_count;
}
