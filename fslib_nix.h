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

            //if (!is_dir(file) && (level == 0 || level == total_level - 1)) {
            if (mode == 1) {
                //printf("match: [%s] %s, %s, %s\n", (isDir(file) ? "D" : " "), fullName, dirName, fileName);
                files.push_back(fullName);
            } else if (mode == 2) {
            //if (is_dir(file) && max_depth >= 0) {
                scandir(fullName, pattern, files, level + 1);
            }

            //if (!is_dir(file) && (level == 0 || level == total_level - 1)) {
            //    // We add file from last pattern level only
            //    //printf("match: [%s] %s, %s, %s\n", (isDir(file) ? "D" : " "), fullName, dirName, fileName);
            //    files.push_back(fullName);
            //}

            //if (is_dir(file) && max_depth >= 0) {
            //    scandir(fullName, pattern, files, level + 1);
            //}

            free(fullName);
        }
    }
    if (errno != 0) {
        // TODO: stderr: error
    }
    closedir(dir);
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