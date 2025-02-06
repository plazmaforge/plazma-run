#include "fsfile.h"
#include "fspath.h"
#include "strlib.h"

#ifdef _WIN32
#include "strlib.h"
#include "wstrlib.h"
#else
#endif

#ifdef _WIN32

static const wchar_t* _lib_fs_wfind_file_ext(const wchar_t* wfile_name) {
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

static int _lib_fs_is_wexec(const wchar_t* wfile_name) {
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

static int _lib_fs_fill_stat_info(const wchar_t* wfile_name, const BY_HANDLE_FILE_INFORMATION* handle_info, lib_stat_t* buf) {

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

////

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

////

// fs-stat

lib_stat_t* lib_fs_stat_new() {
    lib_stat_t* stat = (lib_stat_t*) malloc(sizeof(lib_stat_t));
    if (!stat) {
        return NULL;
    }
    return stat;
}

// fs-file

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


