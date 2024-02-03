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

#include <wchar.h>
#include <windows.h>
//#include <stdio.h>
//#include <stdlib.h>

//#include <fileapi.h>
//#include <aclapi.h>
//#include <io.h>

//#include <winioctl.h>
//#include <direct.h>
//#include <shlobj.h>
//#include <shlwapi.h>

#include "wstrlib.h"
#include "fslib.h"

static int match_file_internal(const char* pattern, const char* name, int mode);

static int match_file_internal(const char* pattern, const char* name);

static wchar_t* getRealPathW(const wchar_t* wpath);

// [allocate]
char* get_normalize_path(const char* dir_name, const char* file_name) { 
    if (is_current_find_path(dir_name)) {
        return strdup(file_name);                  // [allocate]
    } else {
        return get_file_path(dir_name, file_name); // [allocate]
    }
}

// [allocate]
char* get_real_path(const char* path) {
    if (!path) {
        return NULL;
    }
    wchar_t* wpath = char2wchar(path);
    if (!wpath) {
        return NULL;
    }
    wchar_t* wreal_path = getRealPathW(wpath);
    free(wpath);
    if (!wreal_path) {
        return NULL;
    }
    char* real_path = wchar2char(wreal_path);
    free(wreal_path);

    return real_path;
}

const char* get_current_find_path() {
    return "./*"; // Why not '.\*'?
}

int is_current_find_path(const char* path) {
    if (!path) {
        return 0;
    }
    return strcmp(path, "./*") == 0; // Why not '.\*'?
}

////

static int is_dir(WIN32_FIND_DATAW file) {
    return file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

// Convert directory name to WIN32 find path: add '\*'
// [allocate]
static char* get_find_path(const char* dirName) {
    if (dirName == NULL) {
        return NULL;
    }

    int len = strlen(dirName);
    int add = 0;

    if (len == 0) {
        add = 3;
    } else {
        if (dirName[len - 1] == '\\') {
            add = 1;
        }
    }

    len += add;
    char* path = (char*) malloc(len + 1);

    if (add == 3) {
        strcpy(path, "./*"); // Why not '.\*'?
    } else {
        strcpy(path, dirName);
        if (add == 1) {
           strcat(path, "*");
        }
    }
    
    path[len + 1] = '\0';
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

#endif

// https://github.com/Quintus/pathie-cpp/blob/master/src/path.cpp

void scandir_internal(const char* dirName, const char* pattern, std::vector<std::string>& files, int level, int max_depth, int total_level, char* level_pattern) {

    char* path = get_find_path(dirName); // convert 'dirName' to WIN32 find path: add '\*'
    wchar_t* wpath = char2wchar(path);
    //printf("path    : '%s'\n", path);

    WIN32_FIND_DATAW file;
    HANDLE dir = FindFirstFileW(wpath, &file);
    if (dir == INVALID_HANDLE_VALUE /*&& GetLastError() != ERROR_FILE_NOT_FOUND*/) {
        fprintf(stderr, "Directory not found: %s\n", dirName);
        return;
    }
                                                                                         
    while (FindNextFileW(dir, &file) != 0) {

        wchar_t* wfileName = file.cFileName;
        char* fileName = wchar2char(wfileName); // [allocate]

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

            char* fullName = NULL;
            if (is_current_find_path(dirName)) {
               fullName = strdup(fileName);
            } else {
               fullName = get_file_path(dirName, fileName); // [allocate]
            }
            
            //printf("match:fullName: %s\n", fullName);
            //printf("match: [%s] %s, %s, %s\n", (mode == 2 ? "D" : " "), fullName, dirName, fileName);

            if (mode == 1) {
                files.push_back(fullName);
            } else if (mode == 2) {
                scandir(fullName, pattern, files, level + 1);
            }

            free(fullName);
        }

        free(fileName);
    }

    free(path);
    free(wpath);
}

////

int is_dir(fs_dirent_t* dirent) {
    if (!dirent) {
        return 0;
    }
    return dirent->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

fs_dir_t* open_dir(const char* dir_name) {
    if (!dir_name) {
        return NULL;
    }

    char* path = get_find_path(dir_name); // convert 'dir_name' to WIN32 find path: add '\*'
    wchar_t* wpath = char2wchar(path);
    //printf("path    : '%s'\n", path);

    WIN32_FIND_DATAW _file;
    HANDLE _dir = FindFirstFileW(wpath, &_file);
    if (_dir == INVALID_HANDLE_VALUE /*&& GetLastError() != ERROR_FILE_NOT_FOUND*/) {
        fprintf(stderr, "Directory not found: %s\n", dir_name);
        return NULL;
    }

    fs_dir_t* dir = (fs_dir_t*) malloc(sizeof(fs_dir_t));
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

fs_dirent_t* read_dir(fs_dir_t* dir) {
    if (!dir) {
        return NULL;
    }
    if (!dir->dirent) {
        dir->dirent = (fs_dirent_t*) malloc(sizeof(fs_dirent_t));
    }
    if (!dir->dirent) {
        return NULL;
    }

    if (FindNextFileW(dir->ptr, &dir->dirent->fd) == 0) {
        return NULL;
    }

    //dir->dirent->fd = fd;
    //dir->dirent->type = fd->d_type; // TODO: Use Universal type

    char* name = wchar2char(dir->dirent->fd.cFileName); // [allocate]
    dir->dirent->name = name;
    return dir->dirent;
}

int close_dir(fs_dir_t* dir) {
    if (!dir) {
        return 0;
    }
    FindClose(dir->ptr);
    free(dir->dirent->name);
    free(dir);
    return 0;
}

static int match_file_internal(const char* pattern, const char* name, int mode) {

    // PathMatchSpecA
    //printf(" %s -> %s, %d, %d\n", pattern, name, val, res);

    //wchar_t* wpattern = char2wchar(pattern, strlen(pattern));
    //wchar_t* wname = char2wchar(name, strlen(name));

    //return PathMatchSpecW(wname, wpattern);
    //return PathMatchSpecA(name, pattern);

    return match_file(name, pattern); // rotate pattern, name !
}

static int match_file_internal(const char* pattern, const char* name) {
    return match_file_internal(pattern, name, 0);
}

#endif

#endif // PLAZMA_LIB_FSLIB_WIN_H