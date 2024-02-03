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
char* get_normalize_path(const char* dir_name, const char* file_name) {
    return get_file_path(dir_name, file_name); // nothing to do 
}

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

////

int is_dir(fs_dirent_t* dirent) {
    if (!dirent) {
        return 0;
    }
    return dirent->fd->d_type == DT_DIR;
}

fs_dir_t* open_dir(const char* dir_name) {
    if (!dir_name) {
        return NULL;
    }
    DIR* _dir = opendir(dir_name);
    if (!_dir) {
        fprintf(stderr, "Directory not found: %s\n", dir_name);
        return NULL;
    }
    fs_dir_t* dir = (fs_dir_t*) malloc(sizeof(fs_dir_t));
    if (!dir) {
        closedir(_dir);
        return NULL;
    }
    dir->ptr = _dir;
    dir->dirent = NULL;
    return dir;
}

fs_dirent_t* read_dir(fs_dir_t* dir) {
    if (!dir) {
        return NULL;
    }
    struct dirent* fd = readdir(dir->ptr);
    if (!fd) {
        return NULL;
    }
    if (!dir->dirent) {
        dir->dirent = (fs_dirent_t*) malloc(sizeof(fs_dirent_t));
    }
    if (!dir->dirent) {
        return NULL;
    }
    dir->dirent->fd = fd;
    dir->dirent->type = fd->d_type; // TODO: Use Universal type
    dir->dirent->name = fd->d_name;
    return dir->dirent;
}

int close_dir(fs_dir_t* dir) {
    if (!dir) {
        return 0;
    }
    int result = closedir(dir->ptr);
    dir->dirent = NULL;
    free(dir);
    return result;
}

////

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