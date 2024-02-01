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

int scandir_internal2(const char* dir_name, const char* pattern, file_t*** files, int* file_count, int* reserved, int level, int max_depth, int total_level, char* level_pattern) {

    struct dirent* file;
    DIR* dir = opendir(dir_name);
    if (dir == NULL) {
        fprintf(stderr, "Directory not found: %s\n", dir_name);
        return -1;
    }

    errno = 0;
    while ((file = readdir(dir)) != NULL) {

        //printf("readdir    : %p\n", file);

        char* file_name = file->d_name;

        //printf("try [%d] %s, %s, :: %s\n", level, dir_name, file_name, level_pattern);
        if (pattern == NULL || match_file_internal(level_pattern, file_name)) {

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

            char* full_name = get_file_path(dir_name, file_name);

            //printf("match: full_name    : %s\n", full_name);
    
            if (mode == 1) {
                //printf("try  : add_file\n");
                if (*file_count + 1 >= *reserved) { // NULL-terminate array: +1
                    const int inc = 10;	/* increase by this much */
                    *reserved += inc;
                    file_t** list2;
                    //printf("try  : expand_array\n");
                    list2 = (struct file_t **) realloc((file_t*) *files, (*reserved + 1) * sizeof(struct file_t*)); // NULL-terminate array: +1
                    //printf("ok   : expand_array : %d\n", *reserved);
			        if (list2 == NULL) {
                        //printf("err  : malloc\n");
                        free(full_name);
                        closedir(dir);
                        return -1;
			       }
                   *files = list2;
                }
                file_t* f = (file_t*) malloc(sizeof(struct file_t));
                f->name = strdup(full_name);
                //printf("try  : index        : %d\n", *file_count);
                //printf("try  : reserved     : %d\n", *reserved);

                file_t** list = *files;
                //*files[*file_count] = f;
                list[*file_count] = f;

                *file_count = *file_count + 1;
                //printf("try  : file_count   : %d\n", *file_count);
            } else if (mode == 2) {

               //scandir2(full_name, pattern, files, level + 1);

               int level2 = level + 1;
               int max_depth2 = level2;
               if (max_depth2 < 0) {
                  level2 = 0;
               }

               int total_level2 = countPathLevel(pattern); // count path level in pattern
               char* level_pattern2 = getLevelPath(pattern, level2); // [allocate]

               int result = scandir_internal2(full_name, pattern, files, file_count, reserved, level2, max_depth2, total_level2, level_pattern2);
               if (result < 0) {
                // error
               }

               free(level_pattern2);
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