#ifndef PLAZMA_LIB_FSLIB_WIN_H
#define PLAZMA_LIB_FSLIB_WIN_H

#ifdef _WIN32
//#ifndef UNICODE
//#define UNICODE
//#endif

#ifdef UNICODE
#if _WIN32_WINNT >= 0x0600
#define WIN32_FILE_API_NEW
#endif
#endif

#include <windows.h>
#include <errno.h>
#include <wchar.h>
#include <io.h>

//#include <stdio.h>
//#include <stdlib.h>

//#include <fileapi.h>
//#include <aclapi.h>
//#include <io.h>

//#include <winioctl.h>
//#include <direct.h>
//#include <shlobj.h>
//#include <shlwapi.h>

#include "strlib.h"
#include "wstrlib.h"
#include "fslib.h"

static int lib_fs_match_file_internal(const char* pattern, const char* name, int mode);

static int lib_fs_match_file_internal(const char* pattern, const char* name);

static wchar_t* getRealPathW(const wchar_t* wpath);

static wchar_t* getCurrentDirW();

// [allocate]
char* lib_fs_get_normalize_path(const char* dir_name, const char* file_name) { 
    if (lib_fs_is_current_find_path(dir_name)) {
        return strdup(file_name);                     // [allocate]
    } else {
        return lib_fs_get_file_path(dir_name, file_name); // [allocate]
    }
}

// [allocate]
char* lib_fs_get_real_path(const char* path) {
    if (!path) {
        return NULL;
    }
    wchar_t* wpath = char_wchar(path);
    if (!wpath) {
        return NULL;
    }
    wchar_t* wreal_path = getRealPathW(wpath);
    free(wpath);
    if (!wreal_path) {
        return NULL;
    }
    char* real_path = wchar_char(wreal_path);
    free(wreal_path);

    return real_path;
}

// [allocate]
char* lib_fs_get_current_dir() {
    wchar_t* wcurrent_dir = getCurrentDirW();
    if (!wcurrent_dir) {
        return NULL;
    }
    char* current_dir = wchar_char(wcurrent_dir);
    free(wcurrent_dir);
    return current_dir;
}

const char* lib_fs_get_current_find_path() {
    return "./*"; // Why not '.\*'?
}

int lib_fs_is_current_find_path(const char* path) {
    if (!path) {
        return 0;
    }
    return strcmp(path, "./*") == 0; // Why not '.\*'?
}

////

static int _lib_fs_is_dir(WIN32_FIND_DATAW file) {
    return file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

static void _lib_fs_normalize_slash(char* path, size_t len) {
    lib_replace_len(path, len, '/', '\\');
}

// Convert directory name to WIN32 find path: add '\*'
// [allocate]
static char* lib_fs_get_find_path(const char* dirName) {
    if (dirName == NULL) {
        return NULL;
    }

    //printf("get_find_path: input: %s\n", dirName);

    int len = strlen(dirName);
    int add = 0;

    if (len == 0) {
        add = 3;     // add: './*' - current dir
    } else {
        if (dirName[len - 1] == '\\' || dirName[len - 1] == '/') {
            add = 1; // add: '*'
        } else {
            if (dirName[len - 1] != '*') {
                //printf("get_find_path: %s: '*'\n", dirName);
                add = 2; // add: '\*'
            }
        }
    }

    len += add;
    char* path = (char*) malloc(len + 1);

    if (add == 3) {
        strcpy(path, "./*"); // Why not '.\*'?
    } else {
        strcpy(path, dirName);
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

#ifdef WIN32_FILE_API_NEW

// [allocate]
static wchar_t* getRealPathW(HANDLE handle) {
    if (handle == NULL) {
        return NULL;
    }
    DWORD size = GetFinalPathNameByHandleW(handle, NULL, 0, VOLUME_NAME_DOS);
    if (size == 0) {
        SetLastError(ERROR_INVALID_HANDLE);
        return NULL;
    }    
    wchar_t* wpath = (wchar_t*) malloc(sizeof(wchar_t) * size);
    size = GetFinalPathNameByHandleW(handle, wpath, size, VOLUME_NAME_DOS);
    if (size == 0) {
        free(wpath);
        SetLastError(ERROR_INVALID_HANDLE);
        return NULL;
    }
    return wpath;
}

// [allocate]
static wchar_t* getRealPathW(const wchar_t* wpath) {
    if (wpath == NULL) {
        return NULL;
    }
    HANDLE handle = CreateFileW(wpath,
                       0,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
                       NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        //GetLastError();
        return NULL;
    }    
    return getRealPathW(handle);
}

#else

// [allocate]
static wchar_t* getRealPathW(const wchar_t* wpath) {
    if(wpath == NULL) {
        return NULL;
    }
    uint32_t size = GetFullPathNameW(wpath, 0, NULL, NULL);
    if(size == 0) {
        return NULL;
    }

    //PWSTR buf = (PWSTR)_alloca((4 + size) * sizeof(WCHAR));
    //buf[0] = L'\\', buf[1] = L'\\',  buf[2] = L'?', buf[3] = L'\\';
    //size = GetFullPathName(wpath, size, buf + 4, NULL);

    wchar_t buf[size];

    size = GetFullPathNameW(wpath, size, buf, NULL);
    if (size == 0) {
       return NULL; 
    }
    return _wcsdup(buf);
}

static wchar_t* getCurrentDirW() {
    /* Current directory */
    WCHAR buf[MAX_PATH];
    if (GetCurrentDirectoryW(sizeof(buf) / sizeof(WCHAR), buf) != 0) {
      return _wcsdup(buf);
    }
    return NULL;
}

#endif

// https://github.com/Quintus/pathie-cpp/blob/master/src/path.cpp

// void scandir_internal(const char* dirName, const char* pattern, std::vector<std::string>& files, int level, int max_depth, int total_level, char* level_pattern) {

//     char* path = fs_get_find_path(dirName); // convert 'dirName' to WIN32 find path: add '\*'
//     wchar_t* wpath = char_wchar(path);
//     //printf("path    : '%s'\n", path);

//     WIN32_FIND_DATAW file;
//     HANDLE dir = FindFirstFileW(wpath, &file);
//     if (dir == INVALID_HANDLE_VALUE /*&& GetLastError() != ERROR_FILE_NOT_FOUND*/) {
//         fprintf(stderr, "Directory not found: %s\n", dirName);
//         return;
//     }
                                                                                         
//     while (FindNextFileW(dir, &file) != 0) {

//         wchar_t* wfileName = file.cFileName;
//         char* fileName = wchar_char(wfileName); // [allocate]

//         //printf("try [%d] %s, %s, :: %s\n", level, dirName, fileName, level_pattern);
//         if (pattern == NULL || fs_match_file_internal(level_pattern, fileName)) {

//             int mode = 0; // 0 - notning, 1 - file, 2 - dir
//             if (!_fs_is_dir(file)) {
//                 // We add the file from last pattern level only
//                 mode = (level == 0 || level == total_level - 1) ? 1 : 0;
//             } else {
//                 // Recursive if max_depth != -1
//                 mode = max_depth >= 0 ? 2 : 0;
//             }

//             if (mode == 0) {
//                 continue; // notning
//             }

//             char* fullName = NULL;
//             if (fs_is_current_find_path(dirName)) {
//                fullName = strdup(fileName);
//             } else {
//                fullName = fs_get_file_path(dirName, fileName); // [allocate]
//             }
            
//             //printf("match:fullName: %s\n", fullName);
//             //printf("match: [%s] %s, %s, %s\n", (mode == 2 ? "D" : " "), fullName, dirName, fileName);

//             if (mode == 1) {
//                 files.push_back(fullName);
//             } else if (mode == 2) {
//                 scandir(fullName, pattern, files, level + 1);
//             }

//             free(fullName);
//         }

//         free(fileName);
//     }

//     free(path);
//     free(wpath);
// }

////

int lib_fs_is_dirent_dir(lib_fs_dirent_t* dirent) {
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
        return FS_LNK;
    }
    if (dirent->fd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE)) {
        return FS_DIR;
    }
    return FS_REG;
}

lib_fs_dir_t* lib_fs_open_dir(const char* dir_name) {
    if (!dir_name) {
        return NULL;
    }

    char* path = lib_fs_get_find_path(dir_name); // convert 'dir_name' to WIN32 find path: add '\*'
    wchar_t* wpath = char_wchar(path);

    //printf("path    : '%s'\n", path);

    WIN32_FIND_DATAW _file;
    HANDLE _dir = FindFirstFileW(wpath, &_file);

    if (_dir == INVALID_HANDLE_VALUE /*&& GetLastError() != ERROR_FILE_NOT_FOUND*/) {
        fprintf(stderr, "Directory not found: %s\n", dir_name);
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
    dir->dirent->name = wchar_char(dir->dirent->fd.cFileName); // [allocate]

    return dir->dirent;
}

int lib_fs_close_dir(lib_fs_dir_t* dir) {
    if (!dir) {
        return 0;
    }
    FindClose(dir->ptr);
    free(dir->dirent->name);
    free(dir);
    return 0;
}

static int lib_fs_match_file_internal(const char* pattern, const char* name, int mode) {

    // PathMatchSpecA
    //printf(" %s -> %s, %d, %d\n", pattern, name, val, res);

    //wchar_t* wpattern = char_wchar(pattern, strlen(pattern));
    //wchar_t* wname = char_wchar(name, strlen(name));

    //return PathMatchSpecW(wname, wpattern);
    //return PathMatchSpecA(name, pattern);

    return lib_fs_match_file(name, pattern); // rotate pattern, name !
}

static int lib_fs_match_file_internal(const char* pattern, const char* name) {
    return lib_fs_match_file_internal(pattern, name, 0);
}

#endif

#endif // PLAZMA_LIB_FSLIB_WIN_H