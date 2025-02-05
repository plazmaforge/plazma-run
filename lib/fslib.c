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
#include <unistd.h>  /* geuid, chdir, rmdir, access */

#include "fslib_nix.h"
#endif

#include "strlib.h"
#include "wclib.h"
#include "pathlib.h"
#include "fslib.h"

#define LIB_FS_BUF_SIZE 4096 // For file copy

//// fs-math ////

int lib_fs_match_file(const char* name, const char* pattern) {
    return lib_wc_match_file(name, pattern);
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

////

//// fs-check: begin ////

bool lib_fs_exists(const char* file_name) {
    return lib_fs_file_check(file_name, LIB_FILE_CHECK_EXISTS);
}

bool lib_fs_is_reg(const char* file_name) {
    return lib_fs_file_check(file_name, LIB_FILE_CHECK_REG);
}

bool lib_fs_is_dir(const char* file_name) {
    return lib_fs_file_check(file_name, LIB_FILE_CHECK_DIR);
}

bool lib_fs_is_exec(const char* file_name) {
    return lib_fs_file_check(file_name, LIB_FILE_CHECK_EXEC);
}

//// fs-check: end   ////

//// fs-cmd ////

/* POSIX Style                */

int lib_fs_access(const char* path, int mode) {
#ifdef _WIN32
    wchar_t* wpath = lib_mbs_to_wcs(path);
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

int lib_fs_chmod(const char* path, int mode) {
#ifdef _WIN32
    wchar_t* wpath = lib_mbs_to_wcs(path);
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

int lib_fs_mkdir(const char* path, int mode) {
#ifdef _WIN32
    wchar_t* wpath = lib_mbs_to_wcs(path);
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

int lib_fs_chdir(const char* path) {
#ifdef _WIN32
    wchar_t* wpath = lib_mbs_to_wcs(path);
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

int lib_fs_rename(const char* old_path, const char* new_path) {
#ifdef _WIN32
    wchar_t* wold_path;
    wchar_t* wnew_path;
    int retval;
    int save_errno = 0;

    wold_path = lib_mbs_to_wcs(old_path);
    if (!wold_path) {
        errno = EINVAL;
        return -1;
    }

    wnew_path = lib_mbs_to_wcs(new_path);
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

int lib_fs_move(const char* old_path, const char* new_path) {
    if (!old_path || !new_path) {
        errno = EINVAL;
        return -1;
    }

    /*
    If 'new_path' is a directory then
    we create new 'dst_file_name' = 'new_path' + 'base_name'
    where 'base_name' = 'get_base_name'(from 'old_path')
    */

    char* dst_file_name = (char*) new_path;
    bool is_dst_dir = false;
    if (lib_fs_is_dir(new_path)) {
        is_dst_dir = true;
        char* base_name = lib_fs_get_base_name(old_path);          // [allocate]
        dst_file_name = lib_fs_get_file_path(new_path, base_name); // [allocate]
        free(base_name);
    }

    int retval = lib_fs_rename(old_path, dst_file_name);
    if (is_dst_dir) {
        free(dst_file_name);
    }
    return retval;    
}

int lib_fs_copy(const char* src_file_name, const char* dst_file_name) {
    return lib_fs_copy_opts(src_file_name, dst_file_name, LIB_FS_BUF_SIZE);
}

int lib_fs_copy_opts(const char* src_file_name, const char* dst_file_name, int buf_size) {

    FILE* src_file = fopen(src_file_name, "rb");
    if (!src_file) {
        //strerror(errno)
        return -1;
    }

    FILE* dst_file = fopen(dst_file_name, "wb");
    if (!dst_file) {
        fclose(src_file);
        return -1;
    }

    int buffer[buf_size];
    int bytes;

    while ((bytes = fread(buffer, 1, buf_size, src_file)) != 0) {
        if (fwrite(buffer, 1, bytes, dst_file) != bytes) {
            fclose(src_file);
            fclose(dst_file);
            return -1;
        }
    }
    fclose(src_file);
    fclose(dst_file);
    return 0;    
}

int lib_fs_remove(const char* path) {
#ifdef _WIN32
    wchar_t* wpath = lib_mbs_to_wcs(path);
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

int lib_fs_rmdir(const char* path) {
#ifdef _WIN32
    wchar_t* wpath = lib_mbs_to_wcs(path);
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

const wchar_t* _lib_fs_wfind_file_ext(const wchar_t* wfile_name) {
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

int _lib_fs_is_wexec(const wchar_t* wfile_name) {
    if (!wfile_name) {
        return 0;
    }
    const wchar_t* wfile_ext = _lib_fs_wfind_file_ext(wfile_name);
    if (!wfile_ext) {
        return 0;
    }
    return (wcsicmp(wfile_ext, L".exe") == 0 ||
            wcsicmp(wfile_ext, L".com") == 0 ||
            wcsicmp(wfile_ext, L".bat") == 0 ||
            wcsicmp(wfile_ext, L".cmd") == 0);
}

int _lib_fs_is_exec(const char* file_name) {
    if (!file_name) {
        return 0;
    }
    const char* file_ext = lib_fs_find_file_ext(file_name);
    if (!file_ext)
        return 0;
    return (_stricmp(file_ext, ".exe") == 0 ||
            _stricmp(file_ext, ".cmd") == 0 ||
            _stricmp(file_ext, ".bat") == 0 ||
            _stricmp(file_ext, ".com") == 0);
}
#endif

bool lib_fs_file_check(const char* file_name, lib_file_check_t check) {
    if (!file_name) {
        return 0;
    }

#ifdef _WIN32
    DWORD attributes;
    wchar_t *wfile_name;

    wfile_name = lib_mbs_to_wcs(file_name);
    if (!wfile_name)
        return 0;

    attributes = GetFileAttributesW(wfile_name);
    free(wfile_name);

    if (attributes == INVALID_FILE_ATTRIBUTES)
        return 0;

    if (check & LIB_FILE_CHECK_EXISTS)
        return 1;

    if (check & LIB_FILE_CHECK_LNK) {
        if (attributes & FILE_ATTRIBUTE_REPARSE_POINT)
            return 1;
    }

    if (check & LIB_FILE_CHECK_REG) {
        if (!(attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE)))
            return 1;
    }

    if (check & LIB_FILE_CHECK_DIR) {
        if (attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE))
            return 1;
    }

    if (check & LIB_FILE_CHECK_EXEC) {
        return _lib_fs_is_exec(file_name);
    }

    return 0;
#else
    if ((check & LIB_FILE_CHECK_EXISTS) && (access(file_name, F_OK) == 0))
        return 1;

    if ((check & LIB_FILE_CHECK_EXEC) && (access(file_name, X_OK) == 0)) {
        if (getuid() != 0)
            return 1;

        /* For root, on some POSIX systems, access (filename, X_OK)
         * will succeed even if no executable bits are set on the
         * file. We fall through to a stat test to avoid that.
         */
    } else {
        int _check = check;
        _check &= ~LIB_FILE_CHECK_EXEC;
        check = (lib_file_check_t) _check;
    }

    if (check & LIB_FILE_CHECK_LNK) {
        struct stat s;

        if ((lstat(file_name, &s) == 0) && S_ISLNK(s.st_mode))
            return 1;
    }

    if (check & (LIB_FILE_CHECK_REG |
                 LIB_FILE_CHECK_DIR |
                 LIB_FILE_CHECK_EXEC)) {
                    
        struct stat s;

        if (stat(file_name, &s) == 0) {

            if ((check & LIB_FILE_CHECK_REG) && S_ISREG(s.st_mode))
                return 1;

            if ((check & LIB_FILE_CHECK_DIR) && S_ISDIR(s.st_mode))
                return 1;

            /* The extra test for root when access (file, X_OK) succeeds.
             */
            if ((check & LIB_FILE_CHECK_EXEC) &&
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

static int64_t _lib_fs_ftime_to_utime(const FILETIME *ft/*, int32_t* nsec*/) {

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

int _lib_fs_fill_stat_info(const wchar_t* wfile_name, const BY_HANDLE_FILE_INFORMATION* handle_info, lib_stat_t* buf) {

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
        if (_lib_fs_is_wexec(wfile_name))
            buf->st_mode |= S_IXUSR | S_IXGRP | S_IXOTH;
    }

    buf->st_nlink = handle_info->nNumberOfLinks;
    buf->st_size = (((uint64_t) handle_info->nFileSizeHigh) << 32) | handle_info->nFileSizeLow;
    buf->st_ctime = _lib_fs_ftime_to_utime(&handle_info->ftCreationTime);
    buf->st_mtime = _lib_fs_ftime_to_utime(&handle_info->ftLastWriteTime);
    buf->st_atime = _lib_fs_ftime_to_utime(&handle_info->ftLastAccessTime);

    return 0;
}

int _lib_fs_wstat(const wchar_t* wfile_name, lib_stat_t* buf) {

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

    int retval = _lib_fs_fill_stat_info(wfile_name, &handle_info, buf);

    CloseHandle(file_handle);

    return retval;

}

int _lib_fs_stat(const char* file_name, lib_stat_t* buf) {
    wchar_t *wfile_name = lib_mbs_to_wcs(file_name);
    if (!wfile_name) {
        errno = EINVAL;
        return -1;
    }
    int retval = _lib_fs_wstat(wfile_name, buf);
    free(wfile_name);
    return retval;
}

#endif

int lib_fs_stat(const char* path, lib_stat_t* buf) {
    #ifdef _WIN32
    return _lib_fs_stat(path, buf);
    #else
    return stat(path, buf);
    #endif
}

////////////////////////////////

int lib_fs_mkdir_all(const char* path, int mode) {
    char* fn;
    char* p;

    if (lib_stremp(path)) {
        errno = EINVAL;
        return -1;
    }

    /* try to create the full path first */
    if (lib_fs_mkdir(path, mode) == 0) {
        return 0;
    }

    if (errno == EEXIST) {
        if (!lib_fs_file_check(path, LIB_FILE_CHECK_DIR)) {
           errno = ENOTDIR;
           return -1;
        }
        return 0;
    }

    /* walk the full path and try creating each element */
    fn = lib_strdup(path);

    if (lib_fs_is_absolute_path(fn))
        p = (char*) lib_fs_skip_root(fn);
    else
        p = fn;

    do {

        /* skip non-dir separators: search first dir separator */
        while (*p && !LIB_FS_IS_DIR_SEPARATOR(*p))
            p++;

        /* replace dir separartor to NULL-terminal char */
        if (!*p)
            p = NULL;
        else
            *p = '\0';

        if (!lib_fs_file_check(fn, LIB_FILE_CHECK_EXISTS)) {
            if (lib_fs_mkdir(fn, mode) == -1 && errno != EEXIST) {
                int save_errno = errno;
                if (errno != ENOENT || !p) {
                    free(fn);
                    errno = save_errno;
                    return -1;
                }
            }
        } else if (!lib_fs_file_check(fn, LIB_FILE_CHECK_DIR)) {
           free(fn);
           errno = ENOTDIR;
           return -1;
        }

        /* restore dir separator */
        if (p) {
            *p++ = LIB_FS_DIR_SEPARATOR;

            /* skip dir separators: search first non-dir separator char*/
            while (*p && LIB_FS_IS_DIR_SEPARATOR(*p))
                p++;
        }

    } while (p);

    free(fn);

    return 0;
}

/* Human Style                */

int lib_fs_create_dir(const char* path) {
    return lib_fs_mkdir(path, 0775);
}

int lib_fs_create_dir_all(const char* path) {
    return lib_fs_mkdir_all(path, 0775);
}

int lib_fs_remove_file(const char* path) {
    return lib_fs_remove(path);
}

int lib_fs_remove_dir(const char* path) {
    return lib_fs_rmdir(path);
}

////////

lib_stat_t* lib_fs_stat_new() {
    lib_stat_t* stat = (lib_stat_t*) malloc(sizeof(lib_stat_t));
    if (!stat) {
        return NULL;
    }
    return stat;
}

lib_file_t* lib_fs_get_file(const char* file_name) {
    if (!file_name) {
        return NULL;
    }
    char* path = lib_fs_get_real_path(file_name);
    if (!path) {
        return NULL;
    }
    lib_file_t* file = lib_fs_file_new();
    if (!file) {
        return NULL;
    }
    file->name = path;

    lib_stat_t* stat = lib_fs_stat_new();
    if (!stat) {
        return file;
    }
    file->stat = stat;

    /* Get stat info */
    lib_fs_stat(path, file->stat);

    /* Get file type by stat mode */
    file->type = lib_fs_file_get_type_by_mode(stat->st_mode);

    return file;
}

const char* lib_fs_file_get_name(lib_file_t* file) {
    if (!file) {
        return 0;
    }
    return file->name;
}

int lib_fs_file_get_type(lib_file_t* file) {
    if (!file) {
        return 0;
    }
    return file->type;
}

/*
char _file_type(char* path, struct stat Stat) {
    char type = '\0';

    if (S_IFCHR == ( S_IFMT & Stat.st_mode))
        type = 'c';
    else if (S_IFBLK == ( S_IFMT & Stat.st_mode))
         type = 'b';
    else if (S_IFIFO == ( S_IFMT & Stat.st_mode))
        type = 'p';
    else if (S_IFREG == ( S_IFMT & Stat.st_mode))
        type = '-';
    else if (S_IFSOCK == ( S_IFMT & Stat.st_mode))
        type = 's';
    else if (S_IFDIR == ( S_IFMT & Stat.st_mode))
        type = 'd';
    else if (_link(path, Stat))
        type = 'l';
    return type;
}
*/

char lib_fs_file_get_type_char(lib_file_t* file) {
    if (!file) {
        return ' ';
    }

    int type = file->type;

    if (type == LIB_FS_CHR) {
        return 'c';
    } else if (type == LIB_FS_BLK) {
        return 'b';
    } else if (type == LIB_FS_FIFO) {
        return 'p';
    } else if (type == LIB_FS_REG) {
        return '-';
    } else if (type == LIB_FS_SOCK) {
        return 's';
    } else if (type == LIB_FS_DIR) {
        return 'd';
    } else if (type == LIB_FS_LNK) {
        return 'l';
    }

    return ' ';
}

void lib_fs_init_mode(char* mode) {    
    /*  len = 11 + 1  */
    /*  0123456789    */
    /* ' --------- ^' */

    mode[0] = ' ';   /* type  */
    memset(mode + 1, '-', 9);
    mode[10] = ' '; /* access */
    mode[11] = '\0';
}

char* lib_fs_file_add_attr(lib_file_t* file, char* mode) {
    if (!mode) {
        return NULL;
    }
    if (!file || !file->stat) {
        lib_fs_init_mode(mode);
        return mode;
    }
    mode[0] = lib_fs_file_get_type_char(file);
    /* mode[1] - mode[9], mode[10] */
    return lib_fs_file_add_mode(file, mode);
}

char* lib_fs_file_add_mode(lib_file_t* file, char* mode) {
    if (!mode) {
        return NULL;
    }
    if (!file || !file->stat) {
        lib_fs_init_mode(mode);
        return mode;
    }

    int st_mode = file->stat->st_mode;

    /* User */
    mode[1] = st_mode & S_IRUSR ? 'r' : '-';
    mode[2] = st_mode & S_IWUSR ? 'w' : '-';
    mode[3] = st_mode & S_IXUSR ? 'x' : '-';

    /* Group */
    mode[4] = st_mode & S_IRGRP ? 'r' : '-';
    mode[5] = st_mode & S_IWGRP ? 'w' : '-';
    mode[6] = st_mode & S_IXGRP ? 'x' : '-';

    /* Other */
    mode[7] = st_mode & S_IROTH ? 'r' : '-';
    mode[8] = st_mode & S_IWOTH ? 'w' : '-';
    mode[9] = st_mode & S_IXOTH ? 'x' : '-';

    mode[10] = lib_fs_file_get_mode_access(file);

    return mode;
}

char lib_fs_file_get_mode_access(lib_file_t* file) {
    if (!file) {
        return ' ';
    }
    return lib_fs_get_mode_access(file->name);
}

//

uint64_t lib_fs_file_get_size(lib_file_t* file) {
    if (!file || !file->stat) {
        return 0;
    }
    return file->stat->st_size;
}

int lib_fs_file_get_mode(lib_file_t* file) {
    if (!file || !file->stat) {
        return 0;
    }
    return file->stat->st_mode;
}

static time_t _lib_fs_file_get_time(lib_file_t* file, int index) {

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

time_t lib_fs_file_get_atime(lib_file_t* file) {
    return _lib_fs_file_get_time(file, 1);
}

time_t lib_fs_file_get_mtime(lib_file_t* file) {
    return _lib_fs_file_get_time(file, 2);
}

time_t lib_fs_file_get_ctime(lib_file_t* file) {
    return _lib_fs_file_get_time(file, 3);
}

bool lib_fs_file_is_dir(lib_file_t* file) {
    if (!file) {
        return 0;
    }
    return file->type == LIB_FS_DIR;
}

int lib_fs_file_get_type_by_mode(int mode) {    
    #ifdef _WIN32

    /* Base: FS_DIR, FS_REG, FS_LNK only */
    if (S_ISDIR(mode)) {
        return LIB_FS_DIR;
    } else if (S_ISREG(mode)) {
        return LIB_FS_REG;
    } else if (S_ISLNK(mode)) {
        return LIB_FS_LNK;
    }
    return LIB_FS_UNKNOWN;
    #else

    /* Base: FS_DIR, FS_REG, FS_LNK */
    if (S_ISDIR(mode)) {
        return LIB_FS_DIR;
    } else if (S_ISREG(mode)) {
        return LIB_FS_REG;
    } else if (S_ISLNK(mode)) {
        return LIB_FS_LNK;

    /* Other */
    } else if (S_ISFIFO(mode)) {
        return LIB_FS_FIFO;
    } else if (S_ISCHR(mode)) {
        return LIB_FS_CHR;
    } else if (S_ISBLK(mode)) {
        return LIB_FS_BLK;
    } else if (S_ISSOCK(mode)) {
        return LIB_FS_SOCK;
    } else if (S_ISWHT(mode)) {
        return LIB_FS_WHT;
    }
    return LIB_FS_UNKNOWN;
    #endif
}

// [fslib]
lib_file_t* lib_fs_file_new() {
  lib_file_t* file = (lib_file_t*) malloc(sizeof(struct lib_file_t));
  if (!file) {
    return NULL;
  }
  file->type = LIB_FS_UNKNOWN;
  file->name = NULL;
  file->stat = NULL;
  return file;
}

// [fslib]
void lib_fs_file_free(lib_file_t* file) {
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
void lib_fs_files_free(lib_file_t** files) {
    if (!files) {
        return;
    }
    lib_file_t* file = NULL;
    lib_file_t** elements = files;
    while ((file = *elements) != NULL) {
        lib_fs_file_free(file);
        elements++;
    }
    free(files);
}

// [fslib]
int lib_fs_files_init(lib_file_t*** files, size_t size) {
    // NULL-terminate array: +1
    lib_file_t** list = (struct lib_file_t**) calloc(size + 1, sizeof(struct lib_file_t*));
    if (!list) {
        return -1;
    }
    list[size] = NULL;
    *files = list;
    return 0;
}

// [fslib]
int lib_fs_files_reinit(lib_file_t*** files, size_t size) {
    // NULL-terminate array: +1
    lib_file_t** list = (struct lib_file_t **) realloc( /*(lib_file_t *)*/ *files, (size + 1) * sizeof(struct lib_file_t *));
    list[size] = NULL;
    if (!list) {
        return -1;
    }
    *files = list;
    return 0;
}

int lib_fs_is_recursive_ignore_file(const char* file_name) {
    if (!file_name) {
        return 1;
    }
    return (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0);
}

int lib_fs_is_classic_ignore_file(const char* file_name) {
    if (!file_name || file_name[0] == '\0') {
        return 1;
    }
    return file_name[0] == '.';
}

int lib_fs_is_ignore_file(const char* file_name) {
    return 0; //return lib_fs_is_recursive_ignore_file(file_name) || lib_fs_is_classic_ignore_file(file_name);
}

int lib_fs_scandir_internal(const char* dir_name, /*const*/ char** patterns, int pattern_count, lib_file_t*** files, int* file_count, int level, int max_depth, int file_only) {

    DIR* dir = lib_opendir(dir_name);
    if (!dir) {
        return -1;
    }

    //errno = 0;
    struct dirent* dirent;
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

    bool use_stat = true;          // optional
    bool use_dir = !file_only;     // optional
    bool use_force_pattern = true; // optional
    bool is_force_pattern = use_force_pattern && pattern && pattern_count == 1;

    while ((dirent = lib_readdir(dir)) != NULL) {

        char* file_name = dirent->d_name;
        int file_type = dirent->d_type; // lib_fs_get_dirent_type(dirent);                       // dirent type (!)
        int is_recursive_ignore = lib_fs_is_recursive_ignore_file(file_name); // ., ..
        int is_ignore = lib_fs_is_ignore_file(file_name);                     // .git, .svn
        int is_force_ignore = is_recursive_ignore || is_ignore;
        int is_match = is_ignore ? 0 : (pattern == NULL || lib_fs_match_file_internal(pattern, file_name));
        int is_dir_ = dirent->d_type == LIB_FS_DIR; // lib_fs_is_dirent_dir(dirent);                           // dirent type (!)

        if (!is_match && is_dir_ && is_force_pattern && !is_force_ignore) {
            is_match = 1;
        }

        //printf("find [%s] [%s%s] [%d] %s, %s, %s\n", (is_match ? "+" : " "), (is_dir_ ? "D" : " "), (is_ignore ? "I" : " "), level, dir_name, file_name, pattern);

        if (is_match) {

            //printf("match [%d] %s, %s, %s\n", level, dir_name, file_name, pattern);

            int mode = 0; // 0 - notning, 1 - add file/dir, 2 - scandir recursive
            if (!is_dir_) {
                // We add the file from last pattern level only
                if (level == 0 || level == pattern_count - 1 || is_force_pattern) {
                    mode = 1; // add file
                }
            } else {
                // Recursive if max_depth != -1
                if (max_depth >= 0 && !is_recursive_ignore) {
                    mode = 2;
                }
                if (mode == 0 && use_dir) {
                    mode = 1; // add dir
                }
            }

            if (mode == 0) {
                continue; // notning
            }

            char* full_name = lib_fs_get_normalize_path(dir_name, file_name);

            //printf("match: full_name    : %s\n", full_name);

            if (mode == 1) {

                //printf("try   : add_file\n");
                int index = *file_count; // old file_count
                lib_file_t** list = *files;

                if (list[index] == NULL) { // NULL-terminate array: +1
                    const int inc = 10;	/* increase by this much */
                    int size = index + inc;
                    if (lib_fs_files_reinit(files, size) != 0) {                        
                        free(full_name);
                        lib_fs_files_free(*files);
                        //lib_fs_close_dir(dir);
                        lib_closedir(dir);
                        return -1;
                    }
                }

                lib_file_t* file_s = lib_fs_file_new();
                file_s->name = strdup(full_name);
                file_s->type = file_type;                

                if (use_stat) {
                    char* real_path = lib_fs_get_real_path(file_s->name);
                    lib_stat_t* stat = lib_fs_stat_new();
                    lib_fs_stat(real_path, stat);
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

               int result = lib_fs_scandir_internal(full_name, patterns, pattern_count, files, file_count, level + 1, max_depth, file_only);
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

    //lib_fs_close_dir(dir);
    lib_closedir(dir);
    return 0;
}

int lib_fs_compare_by_type(int type1, int type2) {
    if (type1 == LIB_FS_DIR && type2 != LIB_FS_DIR) {
        return -1;
    }

    if (type2 == LIB_FS_DIR && type1 != LIB_FS_DIR) {
        return 1;
    }

    return 0;
}

int lib_fs_compare_by_name(const char* name1, const char* name2) {
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

int lib_fs_compare_by_size(uint64_t size1, uint64_t size2) {
    if (size2 == size1) {
        return 0;
    }
    return size1 > size2 ? 1 : -1; // 
    //return size1 > size2 ? -1 : 1; // ^   
}

int lib_fs_compare_by_time(long time1, long time2) {
    if (time2 == time1) {
        return 0;
    }
    return time1 > time2 ? 1 : -1; // 
    //return time1 > time2 ? -1 : 1; // ^   
}

int lib_fs_compare(const void* v1, const void* v2, lib_file_sort_t file_sort, bool asc, bool is_dir_first) {

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

    lib_file_t* f1 = *((lib_file_t**) v1);
    lib_file_t* f2 = *((lib_file_t**) v2);

    int cmp = 0;
    if (is_dir_first) {
        /* Sort by type before: DIR is priority then other types */
        int type1 = f1->type;
        int type2 = f2->type;
        cmp = lib_fs_compare_by_type(type1, type2);
        if (cmp != 0) {
            return cmp;
        }
    }

    cmp = 0;
    if (file_sort == LIB_FILE_SORT_BY_NAME) {

        /* Sort By name */
        char *name1 = f1->name;
        char *name2 = f2->name;
        cmp = lib_fs_compare_by_name(name1, name2);
    } else if (file_sort == LIB_FILE_SORT_BY_SIZE) {

        /* Sort By size */
        uint64_t size1 = lib_fs_file_get_size(f1);
        uint64_t size2 = lib_fs_file_get_size(f2);
        cmp = lib_fs_compare_by_size(size1, size2);
    } else if (file_sort == LIB_FILE_SORT_BY_TIME) {

        /* Sort By time */
        long time1 = lib_fs_file_get_mtime(f1);
        long time2 = lib_fs_file_get_mtime(f2);
        cmp = lib_fs_compare_by_size(time1, time2);
    }

    return (asc || cmp == 0) ? cmp : (cmp * -1);
}

int lib_fs_file_sort_by_alpha(const void* v1, const void* v2) {
    return lib_fs_compare(v1, v2, LIB_FILE_SORT_BY_NAME, true, false);
}

int lib_fs_file_sort_by_name(const void* v1, const void* v2) {
    return lib_fs_compare(v1, v2, LIB_FILE_SORT_BY_NAME, true, true);
}

int lib_fs_file_sort_by_size(const void* v1, const void* v2) {
    return lib_fs_compare(v1, v2, LIB_FILE_SORT_BY_SIZE, true, false);
}

int lib_fs_file_sort_by_time(const void* v1, const void* v2) {
    return lib_fs_compare(v1, v2, LIB_FILE_SORT_BY_TIME, true, false);
}

////

int lib_fs_file_sort_by_alpha_desc(const void* v1, const void* v2) {
    return lib_fs_compare(v1, v2, LIB_FILE_SORT_BY_NAME, false, false);
}

int lib_fs_file_sort_by_name_desc(const void* v1, const void* v2) {
    return lib_fs_compare(v1, v2, LIB_FILE_SORT_BY_NAME, false, true);
}

int lib_fs_file_sort_by_size_desc(const void* v1, const void* v2) {
    return lib_fs_compare(v1, v2, LIB_FILE_SORT_BY_SIZE, false, false);
}

int fs_file_sort_by_time_desc(const void* v1, const void* v2) {
    return lib_fs_compare(v1, v2, LIB_FILE_SORT_BY_TIME, false, false);
}

int lib_fs_scandir(const char* dir_name, const char* pattern, lib_file_t*** files, int max_depth, int file_only) {
    if (!dir_name) {
        return -1;
    }

    char** patterns = lib_path_split(pattern); // split pattern by level
    int pattern_count = lib_strarrlen(patterns);

    //printf(">>pattern_count : %d\n", pattern_count);
    //printf(">>scandir       : %s\n", dir_name);
    //printf(">>level         : %d\n", level);
    //printf(">>pattern       : %s\n", pattern);

    int file_count = 0;
    int size = 10; // start size

    lib_fs_files_init(files, size);

    lib_fs_scandir_internal(dir_name, /*(const char**)*/ patterns, pattern_count, files, &file_count, 0, max_depth, file_only);

    lib_strarrfree(patterns);

    if (files && file_count > 0) {
        //qsort(*files, file_count, sizeof(struct lib__file_t*), lib_fs_file_sort_by_alpha);
        qsort(*files, file_count, sizeof(struct lib_file_t*), lib_fs_file_sort_by_name);
        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_fs_file_sort_by_size);
        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_fs_file_sort_by_time);

        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_fs_file_sort_by_alpha_desc);
        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_fs_file_sort_by_name_desc);
        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_fs_file_sort_by_size_desc);
        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_fs_file_sort_by_time_desc);

    }
    
    return file_count;
}

// https://github.com/tronkko/dirent/tree/master
// https://github.com/tronkko/dirent/blob/master/examples/scandir.c

// https://github.com/Quintus/pathie-cpp/blob/master/src/path.cpp