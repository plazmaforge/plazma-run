#ifndef PLAZMA_LIB_FSLIB_NIX_H
#define PLAZMA_LIB_FSLIB_NIX_H

#if defined __unix__ || defined __linux__ || (defined __APPLE__ && defined __MACH__)

#include <fnmatch.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h> /* PATH_MAX */

#include "fslib.h"

static int match_file_internal(const char* pattern, const char* name, int mode);

static int match_file_internal(const char* pattern, const char* name);

// [allocate]
char* get_real_path(const char* path) {
    if (!path) {
        return NULL;
    }
    char buf[PATH_MAX];
    if (realpath(path, buf) != buf) {
        return NULL;
    }
    return strdup(buf);
}

const char* get_current_find_path() {
    return ".";
}

int is_current_find_path(const char* path) {
    if (!path) {
        return 0;
    }
    return path[0] == '.';
}

////

static int is_dir(struct dirent* file) {
    if (file == NULL) {
        return false;
    }
    return file->d_type == DT_DIR;
}

void scandir_internal(const char* dirName, const char* pattern, std::vector<std::string>& files, int level, int max_depth, int total_level, char* level_pattern) {

    struct dirent* file;
    DIR* dir = opendir(dirName);
    if (dir == NULL) {
        fprintf(stderr, "Directory not found: %s\n", dirName);
        return;
    }

    errno = 0;
    while ((file = readdir(dir)) != NULL) {

        char* fileName = file->d_name;

        //printf("try [%d] %s, %s, :: %s\n", level, dirName, fileName, level_pattern);
        if (pattern == NULL || match_file_internal(level_pattern, fileName)) {

            int mode = 0; // 0 - notning, 1 - file, 2 - dir
            if (!is_dir(file)) {
                // We add the file from last pattern level only
                mode = (level == 0 || level == total_level - 1) ? 1 : 0;
            } else {
                // Recursive if max_depth != -1
                mode = max_depth >= 0 ? 2 : 0;
            }

            if (mode == 0) {
                continue; // notning
            }

            char* fullName = get_file_path(dirName, fileName);

            //printf("match:fullName: %s\n", fullName);
            //printf("match: [%s] %s, %s, %s\n", (mode == 2 ? "D" : " "), fullName, dirName, fileName);

            if (mode == 1) {
                files.push_back(fullName);
            } else if (mode == 2) {
                scandir(fullName, pattern, files, level + 1);
            }

            free(fullName);
        }
    }
    if (errno != 0) {
        // TODO: stderr: error
    }
    closedir(dir);
}

int scandir_internal2(const char* dir_name, /*const*/ char** patterns, int pattern_count, file_t*** files, int* file_count, /*int* reserved,*/ int level, int max_depth) {

    struct dirent* file;
    DIR* dir = opendir(dir_name);
    if (dir == NULL) {
        fprintf(stderr, "Directory not found: %s\n", dir_name);
        return -1;
    }

    errno = 0;
    const char* pattern = NULL;
    if (patterns)  {
        pattern = patterns[level];
    } 

    while ((file = readdir(dir)) != NULL) {

        char* file_name = file->d_name;

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

            char* full_name = get_file_path(dir_name, file_name);

            //printf("match: full_name    : %s\n", full_name);
    
            if (mode == 1) {

                //printf("try  : add_file\n");
                int index = *file_count; // old file_count
                file_t** list = *files;

                if (list[index] == NULL) { // NULL-terminate array: +1
                    const int inc = 10;	/* increase by this much */
                    int size = index + inc;

                    // printf("try  : expand_array\n");
                    if (files_realloc(files, size) != 0) {
                        //printf("err  : malloc\n");
                        free(full_name);
                        // TODO: free files (!)
                        closedir(dir);
                        return -1;
                    }
                    // printf("ok   : expand_array : %d\n", *reserved);
                }

                file_t* file_s = (file_t*) malloc(sizeof(struct file_t));
                file_s->name = strdup(full_name);

                //printf("try  : index        : %d\n", *file_count);
                //printf("try  : reserved     : %d\n", *reserved);

                list = *files;
                list[index] = file_s; 
                //*files[*file_count] = file_s; // Doesn't work

                *file_count = *file_count + 1;
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
    if (errno != 0) {
        // TODO: stderr: error
    }
    closedir(dir);
    return 0;
}

static int match_file_internal(const char* pattern, const char* name, int mode) {
    //printf(" %s -> %s, %d, %d\n", pattern, name, val, res);
    //return fnmatch(pattern, name, FNM_PERIOD) == 0; // true id '0'

    int val = fnmatch(pattern, name, mode);
    int res = val == 0;
    return res;

    //return match(name, pattern); // rotate pattern, name !
}

static int match_file_internal(const char* pattern, const char* name) {
    return match_file_internal(pattern, name, FNM_PERIOD);
}

#endif

#endif // PLAZMA_LIB_FSLIB_NIX_H