#include <stdlib.h>
#include <stdbool.h>
//#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <wchar.h>
//#include <io.h>

#include "strlib.h"
#include "wstrlib.h"

#else
#include <dirent.h>
#endif

#include "fsdirent.h"

#ifdef _WIN32

static void _lib_fs_normalize_slash(char* path, size_t len) {
    lib_strntrc(path, len, '/', '\\');
}

// Convert directory name to WIN32 find path: add '\*'
// [allocate]
static char* lib_fs_get_find_path(const char* dir_name) {
    if (dir_name == NULL) {
        return NULL;
    }

    //printf("get_find_path: input: %s\n", dir_name);

    int len = strlen(dir_name);
    int add = 0;

    if (len == 0) {
        add = 3;     // add: './*' - current dir
    } else {
        if (dir_name[len - 1] == '\\' || dir_name[len - 1] == '/') {
            add = 1; // add: '*'
        } else {
            if (dir_name[len - 1] != '*') {
                //printf("get_find_path: %s: '*'\n", dir_name);
                add = 2; // add: '\*'
            }
        }
    }

    len += add;
    char* path = (char*) malloc(len + 1);

    if (add == 3) {
        strcpy(path, "./*"); // Why not '.\*'?
    } else {
        strcpy(path, dir_name);
        if (add == 2) {
           strcat(path, "\\");
           strcat(path, "*");
        } else if (add == 1) {
           strcat(path, "*");
        }
        _lib_fs_normalize_slash(path, len);
    }
    
    path[len + 1] = '\0';

    //printf("get_find_path: output: %s\n", path);
            
    return path;
}

////

static bool _lib_fs_is_dir(WIN32_FIND_DATAW file) {
    //return file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    return file.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE);
}

bool lib_fs_is_dirent_dir(lib_fs_dirent_t* dirent) {
    if (!dirent) {
        return 0;
    }
    return dirent->fd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE);
}

int lib_fs_get_dirent_type(lib_fs_dirent_t* dirent) {
    if (!dirent) {
        return 0;
    }
    if (dirent->fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        return LIB_FS_LNK;
    }
    if (dirent->fd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE)) {
        return LIB_FS_DIR;
    }
    return LIB_FS_REG;
}

// [allocate]
lib_fs_dir_t* lib_fs_open_dir(const char* dir_name) {
    if (!dir_name) {
        return NULL;
    }

    char* path = lib_fs_get_find_path(dir_name); // convert 'dir_name' to WIN32 find path: add '\*'
    wchar_t* wpath = lib_mbs_to_wcs(path);

    //printf("path    : '%s'\n", path);

    WIN32_FIND_DATAW _file;
    HANDLE _dir = FindFirstFileW(wpath, &_file);

    if (_dir == INVALID_HANDLE_VALUE /*&& GetLastError() != ERROR_FILE_NOT_FOUND*/) {
        //fprintf(stderr, "Directory not found: %s\n", dir_name);
        return NULL;
    }

    lib_fs_dir_t* dir = (lib_fs_dir_t*) malloc(sizeof(lib_fs_dir_t));
    if (!dir) {
        free(path);
        free(wpath);
        FindClose(_dir);
        return NULL;
    }

    dir->ptr = _dir;
    dir->dirent = NULL;
    return dir;
}

// [allocate]
lib_fs_dirent_t* lib_fs_read_dir(lib_fs_dir_t* dir) {

    if (!dir) {
        return NULL;
    }

    if (!dir->dirent) {
        dir->dirent = (lib_fs_dirent_t*) malloc(sizeof(lib_fs_dirent_t));
    } else {
       if (dir->dirent->name) {
           free(dir->dirent->name);                            // [free]
       }
    }  

    if (!dir->dirent) {
        return NULL;
    }

    dir->dirent->type = 0;
    dir->dirent->name = NULL;

    if (FindNextFileW(dir->ptr, &dir->dirent->fd) == 0) {
        return NULL;
    }

    //dir->dirent->fd = fd;
    dir->dirent->type = lib_fs_get_dirent_type(dir->dirent);
    dir->dirent->name = lib_wcs_to_mbs(dir->dirent->fd.cFileName); // [allocate]

    return dir->dirent;
}

int lib_fs_close_dir(lib_fs_dir_t* dir) {
    if (!dir) {
        return 0;
    }
    FindClose(dir->ptr);
    free(dir->dirent->name);
    // free(dir->dirent)  // ???
    //dir->dirent = NULL; // ???
    free(dir);
    return 0;
}

#else

static inline bool _lib_fs_is_dir_type(int type) {
    return type == DT_DIR;
}

static bool _lib_fs_is_dir(struct dirent* file) {
    if (file == NULL) {
        return false;
    }
    //return file->d_type == DT_DIR;
    return _lib_fs_is_dir_type(file->d_type);
}

bool lib_fs_is_dirent_dir(lib_fs_dirent_t* dirent) {
    if (!dirent) {
        return 0;
    }
    //return dirent->fd->d_type == DT_DIR;
    return _lib_fs_is_dir_type(dirent->fd->d_type);
}

int lib_fs_get_dirent_type(lib_fs_dirent_t* dirent) {
    if (!dirent) {
        return 0;
    }
    return dirent->fd->d_type;
}

// [allocate]
lib_fs_dir_t* lib_fs_open_dir(const char* dir_name) {
    if (!dir_name) {
        return NULL;
    }
    DIR* _dir = opendir(dir_name);
    if (!_dir) {
        //fprintf(stderr, "Directory not found: %s\n", dir_name);
        return NULL;
    }
    lib_fs_dir_t* dir = (lib_fs_dir_t*) malloc(sizeof(lib_fs_dir_t));
    if (!dir) {
        closedir(_dir);
        return NULL;
    }
    dir->ptr = _dir;
    dir->dirent = NULL;
    return dir;
}

// [allocate]
lib_fs_dirent_t* lib_fs_read_dir(lib_fs_dir_t* dir) {
    if (!dir) {
        return NULL;
    }
    struct dirent* fd = readdir(dir->ptr);
    if (!fd) {
        return NULL;
    }
    if (!dir->dirent) {
        dir->dirent = (lib_fs_dirent_t*) malloc(sizeof(lib_fs_dirent_t));
        dir->dirent->type = 0;
        dir->dirent->name = NULL;
    }
    if (!dir->dirent) {
        return NULL;
    }
    dir->dirent->fd = fd;
    dir->dirent->type = lib_fs_get_dirent_type(dir->dirent);
    dir->dirent->name = fd->d_name;
    return dir->dirent;
}

int lib_fs_close_dir(lib_fs_dir_t* dir) {
    if (!dir) {
        return 0;
    }
    int result = closedir(dir->ptr);
    // free(dir->dirent) // ???
    dir->dirent = NULL;
    free(dir);
    return result;
}

#endif