#include <string.h>
#include <stdlib.h>

#ifdef _WIN32

#ifdef UNICODE
#if _WIN32_WINNT >= 0x0600
#define WIN32_FILE_API_NEW
#endif
#endif

#include <windows.h>
#include <errno.h>
#include <wchar.h>
#include <io.h>

#include "wstrlib.h"

#else
#include <limits.h>    /* PATH_MAX  */
#include <unistd.h>    /* getcwd    */
#endif

//#include "strlib.h"   /* strnew, strdup, stremp */
//#include "pathlib.h"  /* is_separator */
#include "fspath.h"

static void _lib_fs_normalize_slash(char* path, size_t len);

#ifdef _WIN32

static wchar_t* getRealPathW(const wchar_t* wpath);

static wchar_t* getCurrentDirW();

//static wchar_t* getUserNameW();

#endif


//////////////////////////////////////////////////////////////
// pathlib
//////////////////////////////////////////////////////////////

static bool lib_path_is_separator(char ch) {
    // cross-platform path separator: '/', '\'
    return ch  == '\\' || ch == '/';
}

//////////////////////////////////////////////////////////////
// strlib
//////////////////////////////////////////////////////////////

/**
 * [allocate]
 */
static char* lib_strnew(size_t size) {
  size_t _size = size * sizeof(char) + 1;
  char* str = (char*) malloc(_size);
  if (!str) {
    return NULL;
  }
  memset(str, 0, _size);
  return str;
}

/**
 * [allocate]
 */
static char* _lib_strndup(const char* src, size_t num) {
  char* dst = lib_strnew(num);
  if (!dst) {
    return NULL;
  }    
  memcpy(dst, src, num);
  //dst[num] = '\0';
  return dst;
}

/**
 * [allocate]
 */
static char* lib_strdup(const char* src) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  return _lib_strndup(src, len);
}

static bool lib_stremp(const char* str) {
    return !str || str[0] == '\0';
}

// strtrc

static char* _lib_strntrc(char* str, size_t num, char from, char to) {
    if (from == to) {
      return str;
    }
    for (size_t i = 0; i < num; i++) {
        if (str[i] == from) {
            str[i] = to;
        }
    }
    return str;
}

static char* lib_strntrc(char* str, size_t num, char from, char to) {
    if (!str) {
        return NULL;
    }
    if (num == 0) {
      return str;
    }
    return _lib_strntrc(str, num, from, to);
}

static char* lib_strtrc(char* str, char from, char to) {
    if (!str) {
        return NULL;
    }
    return lib_strntrc(str, strlen(str), from, to);
}

//////////////////////////////////////////////////////////////

// [allocate]
char* lib_fs_get_file_path(const char* dir_name, const char* file_name) {
    if (!dir_name && !file_name) {
        return NULL;
    }
    if (lib_stremp(dir_name)) {
        return lib_strdup(file_name);
    }

    // TODO: Ups...May be NULL: No file name
    if (lib_stremp(file_name)) {
        return lib_strdup(dir_name);
    }

    int dir_len = strlen(dir_name);
    int file_len = strlen(file_name);
    int sep_len = 0;

    if (lib_path_is_separator(dir_name[dir_len - 1])) { // cross-platform separator
        sep_len++;
    }
    if (lib_path_is_separator(file_name[0])) {          // cross-platform separator
        sep_len++;
    }
    if (sep_len == 2) {
        sep_len = -1; // erase 1 position
    } else if (sep_len == 1) {
        sep_len = 0;  // nothing
    } else {
        sep_len = 1;  // add 1 position between dir and file
    }

    int len = dir_len + sep_len + file_len;

    char* path = lib_strnew(len);
    strcpy(path, dir_name);
    if (sep_len == 1) {
        path[dir_len] = LIB_FS_DIR_SEPARATOR;
        path[dir_len + 1] = '\0'; // ???: Maybe for next strcat
    }
    // shift file_name if erase 1 position
    strcat(path, sep_len == -1 ? file_name + 1 : file_name);
    path[len] = '\0';

    return path;
}

// [allocate]
char* lib_fs_get_normalize_slash(char* path) {
    if (!path) {
        return NULL;
    }
    char* dst = lib_strdup(path);
    if (!dst) {
        return NULL;
    }
    _lib_fs_normalize_slash(dst, strlen(dst));
    return dst;
}

void lib_fs_normalize_slash(char* path) {
    if (!path) {
        return;
    }
    _lib_fs_normalize_slash(path, strlen(path));
}

////

#ifdef _WIN32

static void _lib_fs_normalize_slash(char* path, size_t len) {
    lib_strntrc(path, len, '/', '\\');
}

// [allocate]
char* lib_fs_get_normalize_path(const char* dir_name, const char* file_name) { 
    if (lib_fs_is_current_find_path(dir_name)) {
        return strdup(file_name);                         // [allocate]
    } else {
        return lib_fs_get_file_path(dir_name, file_name); // [allocate]
    }
}

////

// [allocate]
char* lib_fs_get_real_path(const char* path) {
    if (!path) {
        return NULL;
    }
    wchar_t* wpath = lib_mbs_to_wcs(path);
    if (!wpath) {
        return NULL;
    }
    wchar_t* wreal_path = getRealPathW(wpath);
    free(wpath);
    if (!wreal_path) {
        return NULL;
    }
    char* real_path = lib_wcs_to_mbs(wreal_path);
    free(wreal_path);

    return real_path;
}

// [allocate]
char* lib_fs_get_current_dir() {
    wchar_t* wcurrent_dir = getCurrentDirW();
    if (!wcurrent_dir) {
        return NULL;
    }
    char* current_dir = lib_wcs_to_mbs(wcurrent_dir);
    free(wcurrent_dir);
    return current_dir;
}

const char* lib_fs_get_current_find_path() {
    return "./*"; // Why not '.\*'?
}

bool lib_fs_is_current_find_path(const char* path) {
    if (!path) {
        return 0;
    }
    return strcmp(path, "./*") == 0; // Why not '.\*'?
}

///////////////////////////////////////////////////////////////
// INTERNAL
///////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////

#else

static void _lib_fs_normalize_slash(char* path, size_t len) {
    lib_strntrc(path, len, '\\', '/');
}

// [allocate]
char* lib_fs_get_normalize_path(const char* dir_name, const char* file_name) {
    return lib_fs_get_file_path(dir_name, file_name); // nothing to do 
}

////

// [allocate]
char* lib_fs_get_real_path(const char* path) {
    if (!path) {
        return NULL;
    }
    char buf[PATH_MAX];
    if (realpath(path, buf) != buf) {
        return NULL;
    }
    return strdup(buf);
}

// [allocate]
char* lib_fs_get_current_dir() {
  /* Current directory */
  char buf[PATH_MAX];
  //errno = 0;
  if (getcwd(buf, sizeof(buf)) == NULL) {
       return NULL;
  } else {
       return strdup(buf);
  }
  return NULL;
}

const char* lib_fs_get_current_find_path() {
    return ".";
}

bool lib_fs_is_current_find_path(const char* path) {
    if (!path) {
        return 0;
    }
    return path[0] == '.';
}

#endif

