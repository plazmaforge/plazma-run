#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//#include <stdint.h>

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

char* fs_get_normalize_slash(char* path) {
    if (!path) {
        return NULL;
    }
    char* npath = lib_strdup(path);
    _fs_normalize_slash(npath, lib_strlen(npath));
    return npath;
}

void fs_normalize_slash(char* path) {
    if (!path) {
        return;
    }
    _fs_normalize_slash(path, lib_strlen(path));
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

char* fs_get_dir_name(const char* file_name) {

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

const wchar_t* _fs_wfind_file_ext(const wchar_t* wfile_name) {
    if (!wfile_name) {
        return 0;
    }

    const wchar_t* name = wfile_name;
    const wchar_t* dot = NULL;

    do {
        wchar_t* last_dot = wcschr(name, L'.');
        if (last_dot == NULL)
            break;

        dot = last_dot;
        name = &last_dot[1];
    } while (1);

    return dot;
}

int _fs_is_wexecutable(const wchar_t* wfile_name) {
    if (!wfile_name) {
        return 0;
    }
    const wchar_t* wfile_ext = _fs_wfind_file_ext(wfile_name);
    if (!wfile_ext) {
        return 0;
    }
    return (wcsicmp(wfile_ext, L".exe") == 0 ||
            wcsicmp(wfile_ext, L".com") == 0 ||
            wcsicmp(wfile_ext, L".bat") == 0 ||
            wcsicmp(wfile_ext, L".cmd") == 0);
}

int _fs_is_executable(const char* file_name) {
    if (!file_name) {
        return 0;
    }
    const char* file_ext = fs_find_file_ext(file_name);
    if (!file_ext)
        return 0;
    return (_stricmp(file_ext, ".exe") == 0 ||
            _stricmp(file_ext, ".cmd") == 0 ||
            _stricmp(file_ext, ".bat") == 0 ||
            _stricmp(file_ext, ".com") == 0);
}
#endif

int fs_file_check(const char* file_name, fs_file_check_t check) {
    if (!file_name) {
        return 0;
    }

#ifdef _WIN32
    DWORD attributes;
    wchar_t *wfile_name;

    wfile_name = char_wchar(file_name);
    if (!wfile_name)
        return 0;

    attributes = GetFileAttributesW(wfile_name);
    free(wfile_name);

    if (attributes == INVALID_FILE_ATTRIBUTES)
        return 0;

    if (check & FS_FILE_CHECK_EXISTS)
        return 1;

    if (check & FS_FILE_CHECK_IS_SYMLINK) {
        if (attributes & FILE_ATTRIBUTE_REPARSE_POINT)
            return 1;
    }

    if (check & FS_FILE_CHECK_IS_REGULAR) {
        if (!(attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE)))
            return 1;
    }

    if (check & FS_FILE_CHECK_IS_DIR) {
        if (attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE))
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


#ifdef _WIN32

#ifndef S_ISLNK
#  ifdef _S_ISLNK
#    define S_ISLNK(m) _S_ISLNK(m)
#  else
#    ifdef _S_IFLNK
#      define S_ISLNK(m) ((m & S_IFMT) == _S_IFLNK)
#    else
#      ifdef S_IFLNK
#	     define S_ISLNK(m) ((m & S_IFMT) == S_IFLNK)
#      else
#        define _S_IFLNK 0x1200
#        define S_IFLNK _S_IFLNK
#	     define S_ISLNK(m) ((m & S_IFMT) == S_IFLNK)
#      endif
#    endif
#  endif
#endif

#  ifdef _MSC_VER
#    ifndef S_IXUSR
#      define _S_IRUSR _S_IREAD
#      define _S_IWUSR _S_IWRITE
#      define _S_IXUSR _S_IEXEC
#      define S_IRUSR _S_IRUSR
#      define S_IWUSR _S_IWUSR
#      define S_IXUSR _S_IXUSR
#      define S_IRGRP (S_IRUSR >> 3)
#      define S_IWGRP (S_IWUSR >> 3)
#      define S_IXGRP (S_IXUSR >> 3)
#      define S_IROTH (S_IRGRP >> 3)
#      define S_IWOTH (S_IWGRP >> 3)
#      define S_IXOTH (S_IXGRP >> 3)
#    endif
#    ifndef S_ISDIR
#      define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)
#    endif
#    ifndef S_ISREG
#      define S_ISREG(m) (((m) & _S_IFMT) == _S_IFREG)
#    endif
#    ifndef S_ISLNK
#      define S_ISLNK(m) (((m) & _S_IFMT) == _S_IFLNK)
#    endif
#  endif

//

/*
 * https://support.microsoft.com/en-ca/help/167296/how-to-convert-a-unix-time-t-to-a-win32-filetime-or-systemtime
 *
 * FT = UT * 10000000 + 116444736000000000.
 * 
 * UT = (FT - 116444736000000000) / 10000000.
 * 
 * Converts FILETIME to unix epoch time in form
 * of a signed 64-bit integer (can be negative).
 *
 */

static int64_t _fs_ftime_to_utime(const FILETIME *ft/*, int32_t* nsec*/) {

  int64_t result;

  /* 1 unit of FILETIME is 100ns */
  const int64_t hundreds_of_usec_per_sec = 10000000;

  /* The difference between January 1, 1601 UTC (FILETIME epoch) and UNIX epoch
   * in hundreds of nanoseconds.
   */
  const int64_t filetime_unix_epoch_offset = 116444736000000000;

  result = ((int64_t) ft->dwLowDateTime) | (((int64_t) ft->dwHighDateTime) << 32);
  result -= filetime_unix_epoch_offset;

  //if (nsec)
  //  *nsec = (result % hundreds_of_usec_per_sec) * 100;

  return result / hundreds_of_usec_per_sec;
}

int _fs_fill_stat_info(const wchar_t* wfile_name, const BY_HANDLE_FILE_INFORMATION* handle_info, fs_stat_t* buf) {

    if (!wfile_name || !handle_info || !buf) {
        errno = EINVAL;
        return -1;
    }

    buf->st_ino = 0;
    buf->st_mode = 0;
    buf->st_uid = 0;
    buf->st_gid = 0;

    if (handle_info->dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE)) {
        buf->st_mode |= S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
    } else if (handle_info->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        buf->st_mode |= S_IFLNK;
    } else {
        buf->st_mode |= S_IFREG;
    }

    /* S_IFCHR - unsupported */
    /* S_IFIFO - unsupported */
    /* S_IFBLK - unsupported */

    /* MS stat() behaviour,  */
    buf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;

    if ((handle_info->dwFileAttributes & FILE_ATTRIBUTE_READONLY) != FILE_ATTRIBUTE_READONLY)
        buf->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;

    if (!S_ISDIR(buf->st_mode)) {
        if (_fs_is_wexecutable(wfile_name))
            buf->st_mode |= S_IXUSR | S_IXGRP | S_IXOTH;
    }

    buf->st_nlink = handle_info->nNumberOfLinks;
    buf->st_size = (((uint64_t) handle_info->nFileSizeHigh) << 32) | handle_info->nFileSizeLow;
    buf->st_ctime = _fs_ftime_to_utime(&handle_info->ftCreationTime);
    buf->st_mtime = _fs_ftime_to_utime(&handle_info->ftLastWriteTime);
    buf->st_atime = _fs_ftime_to_utime(&handle_info->ftLastAccessTime);

    return 0;
}

int _fs_wstat(const wchar_t* wfile_name, fs_stat_t* buf) {

    if (!wfile_name || !buf) {
        errno = EINVAL;
        return -1;
    }

    BY_HANDLE_FILE_INFORMATION handle_info;
    //FILE_STANDARD_INFO std_info;

    DWORD attributes;
    DWORD open_flags;
    DWORD error_code;
    HANDLE file_handle;
    BOOL success;

    //int is_symlink = 0;
    int is_directory = 0;

    attributes = GetFileAttributesW(wfile_name);

    if (attributes == INVALID_FILE_ATTRIBUTES) {
        error_code = GetLastError();
        errno = to_errno_win(error_code);
        return -1;
    }

    //is_symlink = (attributes & FILE_ATTRIBUTE_REPARSE_POINT) == FILE_ATTRIBUTE_REPARSE_POINT;
    is_directory = (attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;

    open_flags = FILE_ATTRIBUTE_NORMAL;

    // if (for_symlink && is_symlink)
    //  open_flags |= FILE_FLAG_OPEN_REPARSE_POINT;

    if (is_directory)
        open_flags |= FILE_FLAG_BACKUP_SEMANTICS;

    file_handle = CreateFileW(wfile_name, FILE_READ_ATTRIBUTES | FILE_READ_EA,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              NULL, OPEN_EXISTING,
                              open_flags,
                              NULL);

    if (file_handle == INVALID_HANDLE_VALUE) {
        error_code = GetLastError();
        errno = to_errno_win(error_code);
        return -1;
    }

    success = GetFileInformationByHandle(file_handle, &handle_info);
    
    if (!success) {
        error_code = GetLastError();
        errno = to_errno_win(error_code);
        CloseHandle(file_handle);
        return -1;
    }

    int retval = _fs_fill_stat_info(wfile_name, &handle_info, buf);

    CloseHandle(file_handle);

    return retval;

}

int _fs_stat(const char* file_name, fs_stat_t* buf) {
    wchar_t *wfile_name = char_wchar(file_name);
    if (!wfile_name) {
        errno = EINVAL;
        return -1;
    }
    int retval = _fs_wstat(wfile_name, buf);
    free(wfile_name);
    return retval;
}

#endif

int fs_stat(const char* path, fs_stat_t* buf) {
    #ifdef _WIN32
    return _fs_stat(path, buf);
    #else
    return stat(path, buf);
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
    return fs_mkdir(path, 0775);
}

int fs_create_dir_all(const char* path) {
    return fs_mkdir_all(path, 0775);
}

int fs_remove_file(const char* path) {
    return fs_remove(path);
}

int fs_remove_dir(const char* path) {
    return fs_rmdir(path);
}

////////

fs_stat_t* fs_stat_new() {
    fs_stat_t* stat = (fs_stat_t*) malloc(sizeof(fs_stat_t));
    //if (!stat) {
    //    return NULL;
    //}
    return stat;
}

fs_file_t* fs_get_file(const char* file_name) {
    if (!file_name) {
        return NULL;
    }
    char* path = fs_get_real_path(file_name);
    if (!path) {
        return NULL;
    }
    fs_file_t* file = fs_file_new();
    if (!file) {
        return NULL;
    }
    file->name = path;

    fs_stat_t* stat = fs_stat_new();
    if (!stat) {
        return file;
    }
    file->stat = stat;

    /* Get stat info */
    fs_stat(path, file->stat);

    /* Get file type by stat mode */
    file->type = fs_file_get_file_type_by_mode(stat->st_mode);

    return file;
}

const char* fs_file_get_file_name(fs_file_t* file) {
    if (!file) {
        return 0;
    }
    return file->name;
}

int fs_file_get_file_type(fs_file_t* file) {
    if (!file) {
        return 0;
    }
    return file->type;
}

uint64_t fs_file_get_file_size(fs_file_t* file) {
    if (!file || !file->stat) {
        return 0;
    }
    return file->stat->st_size;
}

int fs_file_get_file_mode(fs_file_t* file) {
    if (!file || !file->stat) {
        return 0;
    }
    return file->stat->st_mode;
}

long _fs_file_get_file_time(fs_file_t* file, int index) {

    if (!file || !file->stat) {
        return 0;
    }

   
    #if (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)) && !defined(_WIN32)
    struct timespec timespec_v;

    if (index == 1) {
        timespec_v = file->stat->st_atimespec;
    } else if (index == 2) {
        timespec_v = file->stat->st_mtimespec;
    } else if (index == 3) {
        timespec_v = file->stat->st_ctimespec;
    } else {
        return 0;
    }
    return timespec_v.tv_sec;

    #else

    if (index == 1) {
        return file->stat->st_atime;
    } else if (index == 2) {
        return file->stat->st_mtime;
    } else if (index == 3) {
        return file->stat->st_ctime;
    } else {
        return 0;
    }
    
    #endif    

}

long fs_file_get_file_atime(fs_file_t* file) {
    return _fs_file_get_file_time(file, 1);
}

long fs_file_get_file_mtime(fs_file_t* file) {
    return _fs_file_get_file_time(file, 2);
}

long fs_file_get_file_ctime(fs_file_t* file) {
    return _fs_file_get_file_time(file, 3);
}

int fs_file_is_dir(fs_file_t* file) {
    if (!file) {
        return 0;
    }
    return file->type == FS_DIR;
}

int fs_file_get_file_type_by_mode(int mode) {    
    #ifdef _WIN32
    /* FS_DIR, FS_REG, FS_LNK only */
    if (S_ISDIR(mode)) {
        return FS_DIR;
    } else if (S_ISREG(mode)) {
        return FS_REG;
    } else if (S_ISLNK(mode)) {
        return FS_LNK;
    }
    return FS_UNKNOWN;
    #else
    /* Base */
    if (S_ISDIR(mode)) {
        return FS_DIR;
    } else if (S_ISREG(mode)) {
        return FS_REG;
    } else if (S_ISLNK(mode)) {
        return FS_LNK;
    /* Other */
    } else if (S_ISFIFO(mode)) {
        return FS_FIFO;
    } else if (S_ISCHR(mode)) {
        return FS_CHR;
    } else if (S_ISBLK(mode)) {
        return FS_BLK;
    } else if (S_ISSOCK(mode)) {
        return FS_SOCK;
    } else if (S_ISWHT(mode)) {
        return FS_WHT;
    }
    return FS_UNKNOWN;
    #endif
}

// [fslib]
fs_file_t* fs_file_new() {
  fs_file_t* file = (fs_file_t*) malloc(sizeof(struct fs_file_t));
  if (!file) {
    return NULL;
  }
  file->name = NULL;
  file->stat = NULL;
  return file;
}

// [fslib]
void fs_file_free(fs_file_t* file) {
    if (!file) {
        return;
    }
    if (file->name)  {
        free(file->name);
    }
    if (file->stat)  {
        free(file->stat);
    }            
    free(file);
}

// [fslib]
void fs_files_free(fs_file_t** files) {
    if (!files) {
        return;
    }
    fs_file_t* file = NULL;
    fs_file_t** elements = files;
    while ((file = *elements) != NULL) {
        fs_file_free(file);
        elements++;
    }
    free(files);
}

// [fslib]
int fs_files_init(fs_file_t*** files, size_t size) {
    // NULL-terminate array: +1
    fs_file_t** list = (struct fs_file_t**) calloc(size + 1, sizeof(struct fs_file_t*)); 
    if (!list) {
        return -1;
    }
    list[size] = NULL;
    *files = list;
    return 0;
}

// [fslib]
int fs_files_reinit(fs_file_t*** files, size_t size) {
    // NULL-terminate array: +1
    fs_file_t** list = (struct fs_file_t **) realloc( /*(fs_file_t *)*/ *files, (size + 1) * sizeof(struct fs_file_t *));
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

int fs_scandir_internal(const char* dir_name, /*const*/ char** patterns, int pattern_count, fs_file_t*** files, int* file_count, int level, int max_depth, int file_only) {

    fs_dir_t* dir = fs_open_dir(dir_name);
    if (!dir) {
        return -1;
    }

    fs_dirent_t* file;
    //errno = 0;
    const char* pattern = NULL;

    // BUG (!) 
    if (patterns && level >= 0)  {
        if (level > pattern_count - 1) {
            if (pattern_count == 1) {
                // patterns: '*' or '*.*'
                pattern = patterns[0];
            }
        } else {
            pattern = patterns[level];
        }
    } 

    int use_stat = 1;
    int use_dir = !file_only;

    while ((file = fs_read_dir(dir)) != NULL) {

        char* file_name = file->name;
        int file_type = fs_get_dirent_type(file);
        int is_ignore = fs_is_ignore_file(file_name);
        int is_match = is_ignore ? 0 : (pattern == NULL || fs_match_file_internal(pattern, file_name));
        int is_dir_ = fs_is_dirent_dir(file);

        //printf("find [%s] [%s%s] [%d] %s, %s, %s\n", (is_match ? "+" : " "), (is_dir_ ? "D" : " "), (is_ignore ? "I" : " "), level, dir_name, file_name, pattern);

        if (is_match) {

            //printf("match [%d] %s, %s, %s\n", level, dir_name, file_name, pattern);

            int mode = 0; // 0 - notning, 1 - file, 2 - dir: recursive, 3 - dir: use only
            if (!is_dir_) {
                // We add the file from last pattern level only
                mode = (level == 0 || level == pattern_count - 1) ? 1 : 0;
            } else {
                // Recursive if max_depth != -1
                mode = max_depth >= 0 ? 2 : 0;
                if (mode == 0 && use_dir) {
                    mode = 3;
                }
            }

            if (mode == 0) {
                continue; // notning
            }

            char* full_name = fs_get_normalize_path(dir_name, file_name);

            //printf("match: full_name    : %s\n", full_name);

            if (mode == 1 || (mode == 3)) {

                //printf("try   : add_file\n");
                int index = *file_count; // old file_count
                fs_file_t** list = *files;

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

                fs_file_t* file_s = fs_file_new();
                file_s->name = strdup(full_name);
                file_s->type = file_type;                

                if (use_stat) {
                    char* real_path = fs_get_real_path(file_s->name);
                    fs_stat_t* stat = fs_stat_new();
                    fs_stat(real_path, stat);
                    file_s->stat = stat;
                    free(real_path);
                }

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

            }
                
            if (mode == 2) {
               //printf("try   : add_dir\n");

               int result = fs_scandir_internal(full_name, patterns, pattern_count, files, file_count, level + 1, max_depth, file_only);
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

int fs_compare_by_type(int type1, int type2) {
    if (type1 == FS_DIR && type2 != FS_DIR) {
        return -1;
    }

    if (type2 == FS_DIR && type1 != FS_DIR) {
        return 1;
    }

    return 0;
}

int fs_compare_by_name(const char* name1, const char* name2) {
    if (!name1 && !name2) {
        return 0;
    }
    if (!name1) {
        return -1;
    }
    if (!name2) {
        return 1;
    }
    return strcmp(name1, name2);
}

int fs_compare_by_size(uint64_t size1, uint64_t size2) {
    if (size2 == size1) {
        return 0;
    }
    return size1 > size2 ? 1 : -1; // 
    //return size1 > size2 ? -1 : 1; // ^   
}

int fs_compare_by_time(long time1, long time2) {
    if (time2 == time1) {
        return 0;
    }
    return time1 > time2 ? 1 : -1; // 
    //return time1 > time2 ? -1 : 1; // ^   
}

int fs_compare(const void* v1, const void* v2, fs_file_sort_t file_sort, bool asc, bool is_dir_first) {

    if (file_sort < 1 || file_sort > 3) {
        return 0;
    }

    if (!v1 && !v2) {
        return 0;
    }

    if (!v1) {
        return asc ? -1 : 1;
    }
    if (!v2) {
        return asc ? 1 : -1;
    }

    fs_file_t* f1 = *((fs_file_t**) v1);
    fs_file_t* f2 = *((fs_file_t**) v2);

    int cmp = 0;
    if (is_dir_first) {
        /* Sort by type before: DIR is priority then other types */
        int type1 = f1->type;
        int type2 = f2->type;
        cmp = fs_compare_by_type(type1, type2);
        if (cmp != 0) {
            return cmp;
        }
    }

    cmp = 0;
    if (file_sort == FS_FILE_SORT_BY_NAME) {

        /* Sort By name */
        char *name1 = f1->name;
        char *name2 = f2->name;
        cmp = fs_compare_by_name(name1, name2);
    } else if (file_sort == FS_FILE_SORT_BY_SIZE) {

        /* Sort By size */
        uint64_t size1 = fs_file_get_file_size(f1);
        uint64_t size2 = fs_file_get_file_size(f2);
        cmp = fs_compare_by_size(size1, size2);
    } else if (file_sort == FS_FILE_SORT_BY_TIME) {

        /* Sort By time */
        long time1 = fs_file_get_file_mtime(f1);
        long time2 = fs_file_get_file_mtime(f2);
        cmp = fs_compare_by_size(time1, time2);
    }

    return (asc || cmp == 0) ? cmp : (cmp * -1);
}

int fs_file_sort_by_alpha(const void* v1, const void* v2) {
    return fs_compare(v1, v2, FS_FILE_SORT_BY_NAME, true, false);
}

int fs_file_sort_by_name(const void* v1, const void* v2) {
    return fs_compare(v1, v2, FS_FILE_SORT_BY_NAME, true, true);
}

int fs_file_sort_by_size(const void* v1, const void* v2) {
    return fs_compare(v1, v2, FS_FILE_SORT_BY_SIZE, true, false);
}

int fs_file_sort_by_time(const void* v1, const void* v2) {
    return fs_compare(v1, v2, FS_FILE_SORT_BY_TIME, true, false);
}

////

int fs_file_sort_by_alpha_desc(const void* v1, const void* v2) {
    return fs_compare(v1, v2, FS_FILE_SORT_BY_NAME, false, false);
}

int fs_file_sort_by_name_desc(const void* v1, const void* v2) {
    return fs_compare(v1, v2, FS_FILE_SORT_BY_NAME, false, true);
}

int fs_file_sort_by_size_desc(const void* v1, const void* v2) {
    return fs_compare(v1, v2, FS_FILE_SORT_BY_SIZE, false, false);
}

int fs_file_sort_by_time_desc(const void* v1, const void* v2) {
    return fs_compare(v1, v2, FS_FILE_SORT_BY_TIME, false, false);
}

int fs_scandir(const char* dir_name, const char* pattern, fs_file_t*** files, int max_depth, int file_only) {
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

    fs_scandir_internal(dir_name, /*(const char**)*/ patterns, pattern_count, files, &file_count, 0, max_depth, file_only);

    lib_strafree(patterns);

    if (files && file_count > 0) {
        //qsort(*files, file_count, sizeof(struct fs_file_t*), fs_file_sort_by_alpha);
        qsort(*files, file_count, sizeof(struct fs_file_t*), fs_file_sort_by_name);
        //qsort(*files, file_count, sizeof(struct fs_file_t*), fs_file_sort_by_size);
        //qsort(*files, file_count, sizeof(struct fs_file_t*), fs_file_sort_by_time);

        //qsort(*files, file_count, sizeof(struct fs_file_t*), fs_file_sort_by_alpha_desc);
        //qsort(*files, file_count, sizeof(struct fs_file_t*), fs_file_sort_by_name_desc);
        //qsort(*files, file_count, sizeof(struct fs_file_t*), fs_file_sort_by_size_desc);
        //qsort(*files, file_count, sizeof(struct fs_file_t*), fs_file_sort_by_time_desc);

    }
    
    return file_count;
}

////////

// /* C++ Style - Migration Task */

// //[allocate]
// char* getFilePath(const char* dirName, const char* fileName) {
//     return fs_get_file_path(dirName, fileName);
// }

// //[allocate]
// char* getRealPath(const char* path) {
//     return fs_get_real_path(path);
// }

// std::string getCurrentFindPath() {
//     std::string str(fs_get_current_find_path());
//     return str;
// }

// bool isCurrentFindPath(const char* path) {
//     return fs_is_current_find_path(path);
// }

// ////

// std::vector<std::string> getFiles(const char* dirName) {
//     return getFiles(dirName, NULL);
// }

// std::vector<std::string> getFiles(const char* dirName, const char* pattern) {
//     std::vector<std::string> files;
//     scandir(dirName, pattern, files);
//     return files;
// }

// void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files) {
//     scandir(dirName, pattern, files, FS_SCANDIR_FLAT);
// }

// // Scandir source code
// // https://gist.github.com/saghul/8013376

// void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files, int level) {
//     if (dirName == NULL) {
//         return;
//     }

//     int max_depth = level;
//     if (max_depth < 0) {
//         level = 0;
//     }
//     int total_level = path_count_path_level(pattern); // count path level in pattern
//     char* level_pattern = path_get_level_path(pattern, level); // [allocate]

//     //printf("scandir : %s\n", dirName);
//     //printf("total   : %d\n", total_level);
//     //printf("level   : %d\n", level);
//     //printf("pattern : %s\n", pattern);
//     //printf("select  : %s\n", level_pattern);

//     scandir_internal(dirName, pattern, files, level, max_depth, total_level, level_pattern);

//     free(level_pattern);
// }
