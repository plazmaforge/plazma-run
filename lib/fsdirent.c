#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
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

//// fs-psx: begin

#ifdef _WIN32

static DIR* _opendir(const char* dirname);

static int _closedir(DIR* dir);

static struct dirent* _readdir(DIR* dir);

static int _readdir_r(DIR *dir, struct dirent *entry, struct dirent **result);

static void _rewinddir(DIR* dir);

static WIN32_FIND_DATAW* _dirent_first(DIR* dir);

static WIN32_FIND_DATAW* _dirent_next(DIR* dir);

////

static void _lib_fs_normalize_slash(char* path, size_t len) {
    lib_strntrc(path, len, '/', '\\');
}

// Convert directory name to WIN32 find path: add '\*'
// [allocate]
static char* _find_path(const char* dir_name) {
    if (dir_name == NULL) {
        return NULL;
    }

    //printf("_find_path: input: %s\n", dir_name);

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

    //printf("_find_path: output: %s\n", path);
            
    return path;
}

static int _get_type(WIN32_FIND_DATAW data) {
    DWORD attr = data.dwFileAttributes;
    if ( attr & FILE_ATTRIBUTE_REPARSE_POINT) {
        return LIB_FS_LNK;
    }
    if ( attr & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE)) {
        return LIB_FS_DIR;
    }
    return LIB_FS_REG;
}

static WIN32_FIND_DATAW* _dirent_first(DIR* dir) {
    /* Open directory and retrieve the first entry */

    //dir->handle = FindFirstFileExW(
    //    dir->wpath, FindExInfoStandard, &dir->data,
    //    FindExSearchNameMatch, NULL, 0);

    dir->handle = FindFirstFileW(dir->wpath, &dir->data);
    if (dir->handle == INVALID_HANDLE_VALUE) {
        // error
        return NULL;
    }

	return &dir->data;
}

static WIN32_FIND_DATAW* _dirent_next(DIR* dir) {
    if (FindNextFileW(dir->handle, &dir->data) == 0) {
        return NULL;
    }
    return &dir->data;
}

static struct dirent* _readdir(DIR* dir) {

	struct dirent* entry;
	readdir_r(dirp, &dirp->ent, &entry);

	/* Return pointer to statically allocated directory entry */
	return entry;

    // struct dirent* entry = &dir->ent;
	// entry->d_ino = 0;
    // entry->d_off = -1; /* Not implemented */

    // WIN32_FIND_DATAW* data = _dirent_next(dir);
    // if (data == NULL) {

	//     entry->d_reclen = 0;
    //     entry->d_type = 0;
    //     entry->d_name = NULL;
    //     entry->d_namlen = 1;

    //     return NULL;
    // }

	// entry->d_reclen = sizeof(struct dirent);
    // entry->d_type = _get_type(*data);
    // entry->d_name = lib_wcs_to_mbs(data->cFileName); // [allocate]
    // entry->d_namlen = entry->d_name ? strlen(entry->d_name) : 0;

    return entry;
}

static int _readdir_r(DIR *dir, struct dirent *entry, struct dirent **result) {

    //struct dirent* entry = &dir->ent;
	entry->d_ino = 0;
    entry->d_off = -1; /* Not implemented */

    WIN32_FIND_DATAW* data = _dirent_next(dir);
    if (!data) {

	    //entry->d_reclen = 0;
        //entry->d_type = 0;
        //entry->d_name = NULL;
        //entry->d_namlen = 1;

		*result = NULL;
		return 0;
    }
    char* name = lib_wcs_to_mbs(data->cFileName); // [allocate]
    if (!name) {
        name = lib_wcs_to_mbs(datap->cAlternateFileName); // [allocate]
    }

    if (!name) {
        entry->d_reclen = 0;
        entry->d_type = 0;
        entry->d_name = NULL; // ?
        entry->d_namlen = 0;

	    *result = entry;
	    return 0;
    }

	entry->d_reclen = sizeof(struct dirent);
    entry->d_type = _get_type(*data);
    entry->d_name = lib_wcs_to_mbs(data->cFileName); // [allocate]
    entry->d_namlen = entry->d_name ? strlen(entry->d_name) : 0;

	*result = entry;
	return 0;
}

static void _rewinddir(DIR* dir) {
    /* Check directory pointer */
    if (!dir || dir->handle == INVALID_HANDLE_VALUE || !dir->wpath)
      return;

    /* Release existing search handle */
    FindClose(dir->handle);

    /* Open new search handle */
    _dirent_first(dir);
}

static DIR* _opendir(const char* dirname) {

    char* path = _find_path(dirname); // convert 'dirname' to WIN32 find path: add '\*'
    if (!path) {
        return NULL;
    }

    wchar_t* wpath = lib_mbs_to_wcs(path);
    free(path);
    if (!wpath) {
        return NULL;
    }

    DIR* dir = (DIR*) malloc(sizeof(DIR));
    if (!dir) {
        free(wpath);
        return NULL;
    }
    dir->handle = INVALID_HANDLE_VALUE;
    dir->wpath = wpath;

    //printf("path    : '%s'\n", path);

    //if (handle == INVALID_HANDLE_VALUE /*&& GetLastError() != ERROR_FILE_NOT_FOUND*/) {
    if (!_dirent_first(dir)) {
        // error
        //fprintf(stderr, "Directory not found: %s\n", dirname);
        _closedir(dir);
        return NULL;
    }

    return dir;
}

static int _closedir(DIR* dir) {
    if (dir->handle != INVALID_HANDLE_VALUE) {
        FindClose(dir->handle);
    }    
    free(dir->wpath);
    free(dir);
    return 0;
}

#endif

struct dirent* lib_readdir(DIR* dir) {
    if (!dir) {
        // error
        return NULL;
    }
    #ifdef _WIN32
    return _readdir(dir);
    #else
    return readdir(dir);
    #endif
}

int lib_readdir_r(DIR *dir, struct dirent *entry, struct dirent **result) {
    if (!dir || !entry || !result) {
        // error
        return -1;
    }
    #ifdef _WIN32
    return _readdir_r(dir, entry, result);
    #else
    return readdir_r(dir, entry, result);
    #endif
}
void lib_rewinddir(DIR* dir) {
    if (!dir) {
        // error
        return;
    }
    #ifdef _WIN32
    return _rewinddir(dir);
    #else
    return rewinddir(dir);
    #endif
}

DIR* lib_opendir(const char* dirname) {
    if (!dirname) {
        // error
        return NULL;
    }
    #ifdef _WIN32
    return _opendir(dirname);
    #else
    return opendir(dirname);
    #endif
}

int lib_closedir(DIR* dir) {
    if (!dir) {
        // error
        return 0;
    }
    #ifdef _WIN32
    return _closedir(dir);
    #else
    return closedir(dir);
    #endif
}

//// fs-psx: end

// https://github.com/tronkko/dirent/blob/master/include/dirent.h