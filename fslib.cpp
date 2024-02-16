#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <sys/utime.h>
#include "fslib_win.h"
#else
//#include <utime.h>
//#include <sys/unistd.h>
#include <unistd.h>
#include "fslib_nix.h"
#endif

#include "strlib.h"
#include "wclib.h"
#include "fslib.h"

/* C Style */

// [allocate]
char* fs_get_file_path(const char* dir_name, const char* file_name) {
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

    int dir_len = lib_strlen(dir_name);
    int file_len = lib_strlen(file_name);
    int sep_len = 0;

    if (path_is_path_separator(dir_name[dir_len - 1])) { // cross-platform separator
        sep_len++;
    }
    if (path_is_path_separator(file_name[0])) {          // cross-platform separator
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
        path[dir_len] = FS_DIR_SEPARATOR;
        path[dir_len + 1] = '\0'; // ???: Maybe for next strcat
    }
    // shift file_name if erase 1 position
    strcat(path, sep_len == -1 ? file_name + 1 : file_name);
    path[len] = '\0';

    return path;
}

int fs_match_file(const char* name, const char* pattern) {
    return wc_match_file(name, pattern);
}

////////////////////

#ifdef _WIN32
static int to_errno_win(DWORD error_code) {
  switch (error_code) {
    case ERROR_ACCESS_DENIED:
      return EACCES;
      break;
    case ERROR_ALREADY_EXISTS:
    case ERROR_FILE_EXISTS:
      return EEXIST;
    case ERROR_FILE_NOT_FOUND:
      return ENOENT;
      break;
    case ERROR_INVALID_FUNCTION:
      return EFAULT;
      break;
    case ERROR_INVALID_HANDLE:
      return EBADF;
      break;
    case ERROR_INVALID_PARAMETER:
      return EINVAL;
      break;
    case ERROR_LOCK_VIOLATION:
    case ERROR_SHARING_VIOLATION:
      return EACCES;
      break;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
      return ENOMEM;
      break;
    case ERROR_NOT_SAME_DEVICE:
      return EXDEV;
      break;
    case ERROR_PATH_NOT_FOUND:
      return ENOENT; /* or ELOOP, or ENAMETOOLONG */
      break;
    default:
      return EIO;
      break;
    }
}
#endif

/* General Functions          */

#ifdef _WIN32
int fs_is_drive_path(const char* path) {
    if (!path) {
        return 0;
    }
    /* first [3] chars: 'X:\' */
    return (isalpha (path[0]) 
      && path[1] == ':' 
      && FS_IS_DIR_SEPARATOR(path[2]));
}

int fs_starts_drive_path(const char* path) {
    if (!path) {
        return 0;
    }
    /* first [2] chars: 'X:' */
    return (isalpha (path[0]) && path[1] == ':'); 
}

int fs_is_unc_path(const char* path) {
    /* 
      first [2] chars: '\\'  
      and check next non-dir separator char 
    */
    return (FS_IS_DIR_SEPARATOR(path[0]) &&
        FS_IS_DIR_SEPARATOR(path[1]) &&
        path[2] &&
        !FS_IS_DIR_SEPARATOR(path[2]));
}
#endif

int fs_is_absolute_path(const char* path) {
    if (!path) {
        return 0;
    }

    if (FS_IS_DIR_SEPARATOR(path[0])) {
        return 1;
    }

#ifdef _WIN32
    /* Recognize drive letter on native Windows */
    return fs_is_drive_path(path);
#endif

    return 0;
}

int fs_exists(const char* file_name) {
    return fs_file_check(file_name, FS_FILE_CHECK_EXISTS);
}

int fs_is_regular(const char* file_name) {
    return fs_file_check(file_name, FS_FILE_CHECK_IS_REGULAR);
}

int fs_is_dir(const char* file_name) {
    return fs_file_check(file_name, FS_FILE_CHECK_IS_DIR);
}

int fs_is_executable(const char* file_name) {
    return fs_file_check(file_name, FS_FILE_CHECK_IS_EXECUTABLE);
}

char* fs_get_base_name(const char* file_name) {

    ssize_t base;
    ssize_t last_nonslash;
    size_t len;
    char* retval;

    if (!file_name) {
        return NULL;
    }

    if (file_name[0] == '\0') {
        // return lib_strdup (".");                  // TODO: (?)
        return NULL;
    }

    last_nonslash = strlen(file_name) - 1;

    while (last_nonslash >= 0 && FS_IS_DIR_SEPARATOR(file_name[last_nonslash]))
        last_nonslash--;

    if (last_nonslash == -1)
        /* string only containing slashes */
        // return lib_strdup(FS_DIR_SEPARATOR_STR); // TODO: (?)
        return NULL;

#ifdef _WIN32
    if (last_nonslash == 1 && fs_starts_drive_path(file_name))
        /* string only containing slashes and a drive */
        // return lib_strdup(FS_DIR_SEPARATOR_STR); // TODO: (?)
        return NULL;
#endif
    base = last_nonslash;

    while (base >= 0 && !FS_IS_DIR_SEPARATOR(file_name[base]))
        base--;

#ifdef _WIN32
    if (base == -1 && fs_starts_drive_path(file_name))
        base = 1;
#endif

    len = last_nonslash - base;
    retval = lib_strnew(len);
    memcpy(retval, file_name + (base + 1), len);
    retval[len] = '\0';

    return retval;
}

char* fs_get_dir_name (const char* file_name) {

    char* base;
    size_t len;

    if (!file_name) {
        return NULL;
    }

    base = (char*) strrchr(file_name, FS_DIR_SEPARATOR);

#ifdef _WIN32
    char* q;
    q = (char*) strrchr(file_name, '/');
    if (base == NULL || (q != NULL && q > base))
        base = q;
#endif

    if (!base) {
#ifdef _WIN32
        if (fs_starts_drive_path(file_name)) {
            char drive_colon_dot[4];

            drive_colon_dot[0] = file_name[0];
            drive_colon_dot[1] = ':';
            drive_colon_dot[2] = '.';
            drive_colon_dot[3] = '\0';

            return lib_strdup(drive_colon_dot);
        }
#endif
        return lib_strdup(".");
    }

    /* skip next dir separartors: right-to-left */
    while (base > file_name && FS_IS_DIR_SEPARATOR(*base))
        base--;

#ifdef _WIN32
    
    if (base == file_name + 1 && fs_starts_drive_path(file_name))
        base++;
    else if (fs_is_unc_path(file_name) && base >= file_name + 2) {

        /* \\server\share  -> \\server\share\ */
        /* \\server\share\ -> \\server\share\ */

        const char *p = file_name + 2;

        while (*p && !FS_IS_DIR_SEPARATOR(*p))
            p++;

        if (p == base + 1) {

            /* \\server\share -> \\server\share\ */

            len = strlen(file_name) + 1;
            base = lib_strnew(len);
            strcpy(base, file_name);
            base[len - 1] = FS_DIR_SEPARATOR;
            base[len] = '\0';
            return base;
        }

        if (FS_IS_DIR_SEPARATOR(*p)) {

            /* \\server\share\ -> \\server\share\ */

            p++;

            while (*p && !FS_IS_DIR_SEPARATOR(*p))
                p++;

            if (p == base + 1)
                base++;
        }
    }
#endif

    len = 1 + base - file_name;
    base = lib_strnew(len);
    memmove(base, file_name, len);
    base[len] = '\0';

    return base;
}

char* fs_get_file_name(const char* file_name) {
    return fs_get_base_name(file_name);
}

const char* fs_find_file_ext(const char* file_name) {
    if (!file_name) {
        return NULL;
    }
    return strrchr(file_name, '.');
}

char* fs_get_file_ext(const char* file_name) {
    if (!file_name) {
        return NULL;
    }
    const char* file_ext = fs_find_file_ext(file_name);
    return lib_strdup(file_ext);
}

/**
 * Skips dir separators and returns a pointer to the first non-dir separator char 
 */
const char* fs_skip_dir_separators(const char* path) {
    if (!path) {
        return NULL;
    }
    while (FS_IS_DIR_SEPARATOR(path[0]))
        path++;
    return path;
}

/**
 * Skips non-dir separators and returns a pointer to the first dir separator char 
 */
const char* fs_skip_nondir_separators(const char* path) {
    if (!path) {
        return NULL;
    }
    while (path[0] && !FS_IS_DIR_SEPARATOR(path[0]))
        path++;
    return path;
}

const char* fs_skip_root(const char* path) {
    if (!path) {
        return NULL;
    }
#ifdef _WIN32

    /* Skip \\server\share or //server/share */
    if (fs_is_unc_path(path)) {

        const char* p = strchr(path + 2, FS_DIR_SEPARATOR);
        const char* q = strchr(path + 2, '/');

        if (p == NULL || (q != NULL && q < p))
            p = q;

        if (p && p > path + 2 && p[1]) {
            path = p + 1;
            path = fs_skip_nondir_separators(path);

            /* Possibly skip a backslash after the share name */
            path = fs_skip_dir_separators(path);
            return (char*) path;
        }
    }
#endif

    /* Skip initial slashes */
    if (FS_IS_DIR_SEPARATOR(path[0])) {
        path = fs_skip_dir_separators(path);
        return (char*) path;
    }

#ifdef _WIN32
    /* Skip X:\ */
    if (fs_is_drive_path(path))
        return (char*) path + 3;
#endif

    return NULL;
}

/* POSIX Style                */

int fs_access(const char* path, int mode) {
#ifdef _WIN32
    wchar_t* wpath = char_wchar(path);
    int retval;
    int save_errno;

    if (!wpath) {
        errno = EINVAL;
        return -1;
    }

#ifndef X_OK
#define X_OK 1
#endif

    retval = _waccess(wpath, mode & ~X_OK);
    save_errno = errno;
    free(wpath);
    errno = save_errno;
    return retval;
#else
    return access(path, mode);
#endif
}

int fs_chmod(const char* path, int mode) {
#ifdef _WIN32
    wchar_t* wpath = char_wchar(path);
    int retval;
    int save_errno;

    if (!wpath) {
        errno = EINVAL;
        return -1;
    }    

    retval = _wchmod(wpath, mode);
    save_errno = errno;
    free(wpath);
    errno = save_errno;
    return retval;
#else
    return chmod(path, mode);
#endif
}

int fs_mkdir(const char* path, int mode) {
#ifdef _WIN32
    wchar_t* wpath = char_wchar(path);
    int retval;
    int save_errno;

    if (!wpath) {
        errno = EINVAL;
        return -1;
    }

    retval = _wmkdir(wpath);
    save_errno = errno;
    free(wpath);
    errno = save_errno;
    return retval;
#else
    return mkdir(path, mode);
#endif
}

int fs_chdir(const char* path) {
#ifdef _WIN32
    wchar_t* wpath = char_wchar(path);
    int retval;
    int save_errno;

    if (!wpath) {
        errno = EINVAL;
        return -1;
    }

    retval = _wchdir(wpath);
    save_errno = errno;
    free(wpath);
    errno = save_errno;
    return retval;
#else
    return chdir(path);
#endif
}

int fs_rename(const char* old_path, const char* new_path) {
#ifdef _WIN32
    wchar_t* wold_path;
    wchar_t* wnew_path;
    int retval;
    int save_errno = 0;

    wold_path = char_wchar(old_path);
    if (!wold_path) {
        errno = EINVAL;
        return -1;
    }

    wnew_path = char_wchar(new_path);
    if (!wnew_path) {
        free(wold_path);
        errno = EINVAL;
        return -1;
    }

    if (MoveFileExW(wold_path, wnew_path, MOVEFILE_REPLACE_EXISTING)) {
        retval = 0;
    } else {
        retval = -1;
        save_errno = to_errno_win(GetLastError());
    }

    free(wold_path);
    free(wnew_path);

    errno = save_errno;
    return retval;
#else
    return rename(old_path, new_path);
#endif
}

int fs_remove(const char* path) {
#ifdef _WIN32
    wchar_t* wpath = char_wchar(path);
    int retval;
    int save_errno;

    if (!path) {
        errno = EINVAL;
        return -1;
    }

    retval = _wremove(wpath);
    if (retval == -1) {
        retval = _wrmdir(wpath);
    }
        
    save_errno = errno;
    free(wpath);
    errno = save_errno;    
    return retval;
#else
    return remove(path);
#endif
}

int fs_rmdir(const char* path) {
#ifdef _WIN32
    wchar_t* wpath = char_wchar(path);
    int retval;
    int save_errno;

    if (!wpath) {
        errno = EINVAL;
        return -1;
    }

    retval = _wrmdir(wpath);
    save_errno = errno;
    free(wpath);
    errno = save_errno;
    return retval;
#else
    return rmdir(path);
#endif
}

////////////////////////////////

#ifdef _WIN32
int _fs_is_executable(const char* file_name) {
    if (!file_name) {
        return 0;
    }
    const char* file_ext = fs_find_file_ext(file_name);
    if (!file_ext)
        return 0;

    return (_stricmp(lastdot, ".exe") == 0 ||
        _stricmp(lastdot, ".cmd") == 0 ||
        _stricmp(lastdot, ".bat") == 0 ||
        _stricmp(lastdot, ".com") == 0)
}
#endif

int fs_file_check(const char* file_name, fs_file_check_t check) {
    if (!file_name) {
        return 0;
    }

#ifdef _WIN32
    DWORD attributes;
    wchar_t *wfile_name;

/* stuff missing in std vc6 api */
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES -1
#endif
#ifndef FILE_ATTRIBUTE_DEVICE
#define FILE_ATTRIBUTE_DEVICE 64
#endif

    wfile_name = char_wchar(file_name);
    if (!wfile_name)
        return 0;

    attributes = GetFileAttributesW(wfile_name);
    free(wfile_name);

    if (attributes == INVALID_FILE_ATTRIBUTES)
        return 0;

    if (check & FS_FILE_CHECK_EXISTS)
        return 1;

    if (check & FS_FILE_CHECK_IS_REGULAR) {
        if ((attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE)) == 0)
            return 1;
    }

    if (check & FS_FILE_CHECK_IS_DIR) {
        if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            return 1;
    }

    if (check & FS_FILE_CHECK_IS_EXECUTABLE) {
        return _fs_is_executable(file_name);
    }

    return 0;
#else
    if ((check & FS_FILE_CHECK_EXISTS) && (access(file_name, F_OK) == 0))
        return 1;

    if ((check & FS_FILE_CHECK_IS_EXECUTABLE) && (access(file_name, X_OK) == 0)) {
        if (getuid() != 0)
            return 1;

        /* For root, on some POSIX systems, access (filename, X_OK)
         * will succeed even if no executable bits are set on the
         * file. We fall through to a stat test to avoid that.
         */
    } else {
        int _check = check;
        _check &= ~FS_FILE_CHECK_IS_EXECUTABLE;
        check = (fs_file_check_t) _check;
    }

    if (check & FS_FILE_CHECK_IS_SYMLINK) {
        struct stat s;

        if ((lstat(file_name, &s) == 0) && S_ISLNK(s.st_mode))
            return 1;
    }

    if (check & (FS_FILE_CHECK_IS_REGULAR |
                 FS_FILE_CHECK_IS_DIR |
                 FS_FILE_CHECK_IS_EXECUTABLE)) {
                    
        struct stat s;

        if (stat(file_name, &s) == 0) {

            if ((check & FS_FILE_CHECK_IS_REGULAR) && S_ISREG(s.st_mode))
                return 1;

            if ((check & FS_FILE_CHECK_IS_DIR) && S_ISDIR(s.st_mode))
                return 1;

            /* The extra test for root when access (file, X_OK) succeeds.
             */
            if ((check & FS_FILE_CHECK_IS_EXECUTABLE) &&
                ((s.st_mode & S_IXOTH) ||
                 (s.st_mode & S_IXUSR) ||
                 (s.st_mode & S_IXGRP)))
                return 1;
        }
    }

    return 0;
#endif
}

////////////////////////////////

int fs_mkdir_all(const char* path, int mode) {
    char* fn;
    char* p;

    if (lib_stremp(path)) {
        errno = EINVAL;
        return -1;
    }

    /* try to create the full path first */
    if (fs_mkdir(path, mode) == 0) {
        return 0;
    }

    if (errno == EEXIST) {
        if (!fs_file_check(path, FS_FILE_CHECK_IS_DIR)) {
           errno = ENOTDIR;
           return -1;
        }
        return 0;
    }

    /* walk the full path and try creating each element */
    fn = lib_strdup(path);

    if (fs_is_absolute_path(fn))
        p = (char*) fs_skip_root(fn);
    else
        p = fn;

    do {

        /* skip non-dir separators: search first dir separator */
        while (*p && !FS_IS_DIR_SEPARATOR(*p))
            p++;

        /* replace dir separartor to NULL-terminal char */
        if (!*p)
            p = NULL;
        else
            *p = '\0';

        if (!fs_file_check(fn, FS_FILE_CHECK_EXISTS)) {
            if (fs_mkdir(fn, mode) == -1 && errno != EEXIST) {
                int save_errno = errno;
                if (errno != ENOENT || !p) {
                    free(fn);
                    errno = save_errno;
                    return -1;
                }
            }
        } else if (!fs_file_check(fn, FS_FILE_CHECK_IS_DIR)) {
           free(fn);
           errno = ENOTDIR;
           return -1;
        }

        /* restore dir separator */
        if (p) {
            *p++ = FS_DIR_SEPARATOR;

            /* skip dir separators: search first non-dir separator char*/
            while (*p && FS_IS_DIR_SEPARATOR(*p))
                p++;
        }

    } while (p);

    free(fn);

    return 0;
}

/* Human Style                */

int fs_create_dir(const char* path) {
    return fs_mkdir(path, 0777);
}

int fs_create_dir_all(const char* path) {
    return fs_mkdir_all(path, 0777);
}

int fs_remove_file(const char* path) {
    return fs_remove(path);
}

int fs_remove_dir(const char* path) {
    return fs_rmdir(path);
}

////////

// [fslib]
file_t* fs_file_new() {
  file_t* file = (file_t*) malloc(sizeof(struct file_t));
  if (!file) {
    return NULL;
  }
  file->name = NULL;
  return file;
}

// [fslib]
void fs_file_free(file_t* file) {
    if (!file) {
        return;
    }
    //free(file->name);
    free(file);
}

// [fslib]
void fs_files_free(file_t** files) {
    if (!files) {
        return;
    }
    file_t* file = NULL;
    file_t** elements = files;
    while ((file = *elements) != NULL) {
        fs_file_free(file);
        elements++;
    }
    free(files);
}

// [fslib]
int fs_files_init(file_t*** files, size_t size) {
    // NULL-terminate array: +1
    file_t** list = (struct file_t**) calloc(size + 1, sizeof(struct file_t*)); 
    if (!list) {
        return -1;
    }
    list[size] = NULL;
    *files = list;
    return 0;
}

// [fslib]
int fs_files_reinit(file_t*** files, size_t size) {
    // NULL-terminate array: +1
    file_t** list = (struct file_t **) realloc( /*(file_t *)*/ *files, (size + 1) * sizeof(struct file_t *));
    list[size] = NULL;
    if (!list) {
        return -1;
    }
    *files = list;
    return 0;
}

int fs_is_ignore_file(const char* file_name) {
    if (!file_name) {
        return 1;
    }
    return (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0);
}

int fs_scandir_internal(const char* dir_name, /*const*/ char** patterns, int pattern_count, file_t*** files, int* file_count, int level, int max_depth) {

    fs_dir_t* dir = fs_open_dir(dir_name);
    if (!dir) {
        return -1;
    }

    fs_dirent_t* file;
    //errno = 0;
    const char* pattern = NULL;
    if (patterns)  {
        pattern = patterns[level];
    } 

    while ((file = fs_read_dir(dir)) != NULL) {

        char* file_name = file->name;
        int is_ignore = fs_is_ignore_file(file_name);
        int is_match = is_ignore ? 0 : (pattern == NULL || fs_match_file_internal(pattern, file_name));
        int is_dir_ = fs_is_dirent_dir(file);

        //printf("find [%s] [%s%s] [%d] %s, %s, %s\n", (is_match ? "+" : " "), (is_dir_ ? "D" : " "), (is_ignore ? "I" : " "), level, dir_name, file_name, pattern);

        if (is_match) {

            //printf("match [%d] %s, %s, %s\n", level, dir_name, file_name, pattern);

            int mode = 0; // 0 - notning, 1 - file, 2 - dir
            if (!is_dir_) {
                // We add the file from last pattern level only
                mode = (level == 0 || level == pattern_count - 1) ? 1 : 0;
            } else {
                // Recursive if max_depth != -1
                mode = max_depth >= 0 ? 2 : 0;
            }

            if (mode == 0) {
                continue; // notning
            }

            char* full_name = fs_get_normalize_path(dir_name, file_name);

            //printf("match: full_name    : %s\n", full_name);
    
            if (mode == 1) {

                //printf("try   : add_file\n");
                int index = *file_count; // old file_count
                file_t** list = *files;

                if (list[index] == NULL) { // NULL-terminate array: +1
                    const int inc = 10;	/* increase by this much */
                    int size = index + inc;
                    if (fs_files_reinit(files, size) != 0) {                        
                        free(full_name);
                        fs_files_free(*files);
                        fs_close_dir(dir);
                        return -1;
                    }
                }

                file_t* file_s = fs_file_new(); // (file_t*) malloc(sizeof(struct file_t));
                file_s->name = strdup(full_name);

                //printf("try  : index        : %d\n", *file_count);
                //printf("try  : reserved     : %d\n", *reserved);

                list = *files;
                list[index] = file_s; 
                //*files[*file_count] = file_s; // Doesn't work

                *file_count = *file_count + 1;
                 
                // Shift NULL marker (size + 1): for allocation without fulling NULL
                index = *file_count; // new file_count
                if (list[index] != NULL) {
                    list[index] = NULL;
                }

                //printf("try  : file_count   : %d\n", *file_count);
            } else if (mode == 2) {
               //printf("try   : add_dir\n");

               int result = fs_scandir_internal(full_name, patterns, pattern_count, files, file_count, level + 1, max_depth);
               if (result < 0) {
                 //printf("err  : %d\n", result);
                 // error
               }
            }

            free(full_name);
        }
    }
    //if (errno != 0) {
        // TODO: stderr: error
    //}
    fs_close_dir(dir);
    return 0;
}

int fs_scandir(const char* dir_name, const char* pattern, file_t*** files, int max_depth) {
    if (!dir_name) {
        return -1;
    }

    char** patterns = path_split_path(pattern); // split pattern by level
    int pattern_count = lib_stralen(patterns);

    //printf(">>pattern_count : %d\n", pattern_count);
    //printf(">>scandir       : %s\n", dir_name);
    //printf(">>level         : %d\n", level);
    //printf(">>pattern       : %s\n", pattern);

    int file_count = 0;
    int size = 10; // start size

    fs_files_init(files, size);

    fs_scandir_internal(dir_name, /*(const char**)*/ patterns, pattern_count, files, &file_count, 0, max_depth);

    lib_strafree(patterns);

    return file_count;
}

////////

/* C++ Style - Migration Task */

//[allocate]
char* getFilePath(const char* dirName, const char* fileName) {
    return fs_get_file_path(dirName, fileName);
}

//[allocate]
char* getRealPath(const char* path) {
    return fs_get_real_path(path);
}

std::string getCurrentFindPath() {
    std::string str(fs_get_current_find_path());
    return str;
}

bool isCurrentFindPath(const char* path) {
    return fs_is_current_find_path(path);
}

////

std::vector<std::string> getFiles(const char* dirName) {
    return getFiles(dirName, NULL);
}

std::vector<std::string> getFiles(const char* dirName, const char* pattern) {
    std::vector<std::string> files;
    scandir(dirName, pattern, files);
    return files;
}

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files) {
    scandir(dirName, pattern, files, FS_SCANDIR_FLAT);
}

// Scandir source code
// https://gist.github.com/saghul/8013376

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files, int level) {
    if (dirName == NULL) {
        return;
    }

    int max_depth = level;
    if (max_depth < 0) {
        level = 0;
    }
    int total_level = path_count_path_level(pattern); // count path level in pattern
    char* level_pattern = path_get_level_path(pattern, level); // [allocate]

    //printf("scandir : %s\n", dirName);
    //printf("total   : %d\n", total_level);
    //printf("level   : %d\n", level);
    //printf("pattern : %s\n", pattern);
    //printf("select  : %s\n", level_pattern);

    scandir_internal(dirName, pattern, files, level, max_depth, total_level, level_pattern);

    free(level_pattern);
}
