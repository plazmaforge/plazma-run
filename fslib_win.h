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

/*
std::string getCurrentFindPath() {
    return get_current_find_path();
    //return "./*"; // Why not '.\*'?
}

bool isCurrentFindPath(const char* path) {
    return is_current_find_path(path)
    // if (path == NULL) {
    //     return false;
    // }
    // return strcmp(path, "./*") == 0; // Why not '.\*'?
}
*/

////

static int is_dir(WIN32_FIND_DATAW file) {
    return file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

// Convert directory name to WIN32 find path: add '\*'
static char* getFindPath(const char* dirName) {
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


// static char* _getRealPath(const char* path) {
//     if (path == NULL) {
//         return NULL;
//     }
//     wchar_t* wpath = char2wchar(path);
//     if (wpath == NULL) {
//         return NULL;
//     }
//     wchar_t* wreal_path = getRealPath(wpath);
//     free(wpath);
//     if (wreal_path == NULL) {
//         return NULL;
//     }
//     char* real_path = wchar2char(wreal_path);
//     free(wreal_path);

//     return real_path;
// }

// https://github.com/Quintus/pathie-cpp/blob/master/src/path.cpp

void scandir_internal(const char* dirName, const char* pattern, std::vector<std::string>& files, int level, int max_depth, int total_level, char* level_pattern) {

    char* path = getFindPath(dirName); // convert 'dirName' to WIN32 find path: add '\*'
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

            char* fullName = NULL;
            if (is_current_find_path(dirName)) {
               fullName = strdup(fileName);
            } else {
               fullName = get_file_path(dirName, fileName); // [allocate]
            }            
            //printf("match:fullName: %s\n", fullName);

            if (!is_dir(file) && (level == 0 || level == total_level - 1)) {
                // We add file from last pattern level only
                //printf("match: [%s] %s, %s, %s\n", (isDir(file) ? "D" : " "), fullName, dirName, fileName);
                files.push_back(fullName);
            }
 
            if (is_dir(file) && max_depth >= 0) {
                scandir(fullName, pattern, files, level + 1);
            }

            free(fullName);
        }

        free(fileName);
    }

    free(path);
    free(wpath);
}

static int match_file_internal(const char* pattern, const char* name, int mode) {

    // PathMatchSpecA
    //printf(" %s -> %s, %d, %d\n", pattern, name, val, res);

    //wchar_t* wpattern = char2wchar(pattern, strlen(pattern));
    //wchar_t* wname = char2wchar(name, strlen(name));

    //return PathMatchSpecW(wname, wpattern);
    //return PathMatchSpecA(name, pattern);

    return match(name, pattern); // rotate pattern, name !
}

static int match_file_internal(const char* pattern, const char* name) {
    return match_file_internal(pattern, name, 0);
}

#endif

#endif // PLAZMA_LIB_FSLIB_WIN_H