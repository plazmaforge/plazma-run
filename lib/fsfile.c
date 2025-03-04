#include "fsfile.h"
#include "fspath.h"
#include "strlib.h"

#ifdef _WIN32
#include "strlib.h"
#include "wstrlib.h"
#else
#endif

#ifdef _WIN32

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

static int _lib_fs_fill_stat(const wchar_t* wpath, const BY_HANDLE_FILE_INFORMATION* info, lib_stat_t* buf) {

    if (!wpath || !info || !buf) {
        errno = EINVAL;
        return -1;
    }

    buf->st_ino = 0;
    buf->st_mode = 0;
    buf->st_uid = 0;
    buf->st_gid = 0;

    if (info->dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE)) {
        buf->st_mode |= S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
    } else if (info->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        buf->st_mode |= S_IFLNK;
    } else {
        buf->st_mode |= S_IFREG;
    }

    /* S_IFCHR - unsupported */
    /* S_IFIFO - unsupported */
    /* S_IFBLK - unsupported */

    /* MS stat() behaviour,  */
    buf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;

    if ((info->dwFileAttributes & FILE_ATTRIBUTE_READONLY) != FILE_ATTRIBUTE_READONLY)
        buf->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;


    // Check eecutable mode by file extension
    if (!S_ISDIR(buf->st_mode)) {
        if (lib_fs_is_wexec_ext(wpath))
            buf->st_mode |= S_IXUSR | S_IXGRP | S_IXOTH;
    }

    buf->st_nlink = info->nNumberOfLinks;
    buf->st_size = (((uint64_t) info->nFileSizeHigh) << 32) | info->nFileSizeLow;
    buf->st_ctime = _lib_fs_ftime_to_utime(&info->ftCreationTime);
    buf->st_mtime = _lib_fs_ftime_to_utime(&info->ftLastWriteTime);
    buf->st_atime = _lib_fs_ftime_to_utime(&info->ftLastAccessTime);

    return 0;
}

int _lib_fs_wstat(const wchar_t* wpath, lib_stat_t* buf) {

    if (!wpath || !buf) {
        errno = EINVAL;
        return -1;
    }

    BY_HANDLE_FILE_INFORMATION info;
    //FILE_STANDARD_INFO std_info;

    DWORD attributes;
    DWORD open_flags;
    DWORD error_code;
    HANDLE handle;
    BOOL success;

    //int is_symlink = 0;
    int is_directory = 0;

    attributes = GetFileAttributesW(wpath);

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

    handle = CreateFileW(wpath, FILE_READ_ATTRIBUTES | FILE_READ_EA,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              NULL, OPEN_EXISTING,
                              open_flags,
                              NULL);

    if (handle == INVALID_HANDLE_VALUE) {
        error_code = GetLastError();
        errno = to_errno_win(error_code);
        return -1;
    }

    success = GetFileInformationByHandle(handle, &info);
    
    if (!success) {
        error_code = GetLastError();
        errno = to_errno_win(error_code);
        CloseHandle(handle);
        return -1;
    }

    int retval = _lib_fs_fill_stat(wpath, &info, buf);

    CloseHandle(handle);

    return retval;

}

int _lib_fs_stat(const char* path, lib_stat_t* buf) {
    wchar_t *wpath = lib_mbs_to_wcs(path);
    if (!wpath) {
        errno = EINVAL;
        return -1;
    }
    int retval = _lib_fs_wstat(wpath, buf);
    free(wpath);
    return retval;
}

#endif

// fs

lib_file_t* lib_fs_get_file(const char* path) {
    return lib_file_get(path);
}

void lib_fs_init_mode(char* mode) {
    return lib_file_init_mode(mode);
}

int lib_fs_get_type_by_mode(int mode) {
    return lib_file_get_type_by_mode(mode);
}

// fs-stat

int lib_fs_stat(const char* path, lib_stat_t* buf) {
    if (!path && !buf) {
        errno = EINVAL;
        return -1;
    }
    #ifdef _WIN32
    return _lib_fs_stat(path, buf);
    #else
    return stat(path, buf);
    #endif
}

lib_stat_t* lib_fs_stat_new() {
    lib_stat_t* stat = (lib_stat_t*) malloc(sizeof(lib_stat_t));
    if (!stat) {
        return NULL;
    }
    return stat;
}

////

lib_file_t* lib_file_new() {
  lib_file_t* file = (lib_file_t*) malloc(sizeof(struct lib_file_t));
  if (!file) {
    return NULL;
  }
  file->type = LIB_FS_UNKNOWN;
  file->name = NULL;
  file->stat = NULL;
  return file;
}

void lib_file_free(lib_file_t* file) {
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

void lib_files_free(lib_file_t** files) {
    if (!files) {
        return;
    }
    lib_file_t* file = NULL;
    lib_file_t** list = files;
    while ((file = *list) != NULL) {
        lib_file_free(file);
        list++;
    }
    free(files);
}

int lib_files_init(lib_file_t*** files, size_t size) {
    if (!files) {
        return -1;
    }
    // NULL-terminate array: +1
    lib_file_t** list = (struct lib_file_t**) calloc(size + 1, sizeof(struct lib_file_t*));
    if (!list) {
        return -1;
    }
    list[size] = NULL;
    *files = list;
    return 0;
}

int lib_files_reinit(lib_file_t*** files, size_t size) {
    if (!files) {
        return -1;
    }
    // NULL-terminate array: +1
    lib_file_t** list = (struct lib_file_t **) realloc( /*(lib_file_t *)*/ *files, (size + 1) * sizeof(struct lib_file_t *));
    list[size] = NULL;
    if (!list) {
        return -1;
    }
    *files = list;
    return 0;
}

// fs-file

lib_file_t* lib_file_get(const char* path) {
    if (!path) {
        return NULL;
    }
    char* rpath = lib_fs_get_real_path(path);
    if (!rpath) {
        return NULL;
    }
    lib_file_t* file = lib_file_new();
    if (!file) {
        return NULL;
    }
    file->name = rpath;

    lib_stat_t* stat = lib_fs_stat_new();
    if (!stat) {
        return file;
    }
    file->stat = stat;

    /* Get stat info */
    lib_fs_stat(rpath, file->stat);

    /* Get file type by stat mode */
    file->type = lib_fs_get_type_by_mode(stat->st_mode);

    return file;
}

const char* lib_file_get_name(lib_file_t* file) {
    if (!file) {
        return 0;
    }
    return file->name;
}

int lib_file_get_type(lib_file_t* file) {
    if (!file) {
        return 0;
    }
    return file->type;
}

int lib_file_get_type_by_mode(int mode) {
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

char lib_file_get_type_char(lib_file_t* file) {
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

void lib_file_init_mode(char* mode) {    
    /*  len = 11 + 1  */
    /*  0123456789    */
    /* ' --------- ^' */

    mode[0] = ' ';   /* type  */
    memset(mode + 1, '-', 9);
    mode[10] = ' '; /* access */
    mode[11] = '\0';
}

char* lib_file_add_attr(lib_file_t* file, char* mode) {
    if (!mode) {
        return NULL;
    }
    if (!file || !file->stat) {
        lib_fs_init_mode(mode);
        return mode;
    }
    mode[0] = lib_file_get_type_char(file);
    /* mode[1] - mode[9], mode[10] */
    return lib_file_add_mode(file, mode);
}

char* lib_file_add_mode(lib_file_t* file, char* mode) {
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

    mode[10] = lib_file_get_amode(file);

    return mode;
}

char lib_file_get_amode(lib_file_t* file) {
    if (!file) {
        return ' ';
    }
    return lib_file_get_amode_by_path(file->name);
}

//

uint64_t lib_file_get_size(lib_file_t* file) {
    if (!file || !file->stat) {
        return 0;
    }
    return file->stat->st_size;
}

int lib_file_get_mode(lib_file_t* file) {
    if (!file || !file->stat) {
        return 0;
    }
    return file->stat->st_mode;
}

int lib_file_get_nlink(lib_file_t* file) {
    if (!file || !file->stat) {
        return 0;
    }
    return file->stat->st_nlink;
}

static time_t _lib_file_get_time(lib_file_t* file, int index) {

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

time_t lib_file_get_atime(lib_file_t* file) {
    return _lib_file_get_time(file, 1);
}

time_t lib_file_get_mtime(lib_file_t* file) {
    return _lib_file_get_time(file, 2);
}

time_t lib_file_get_ctime(lib_file_t* file) {
    return _lib_file_get_time(file, 3);
}

bool lib_file_is_dir(lib_file_t* file) {
    if (!file) {
        return 0;
    }
    return file->type == LIB_FS_DIR;
}

// fs-file-compare

int lib_file_compare_by_type(int type1, int type2) {
    if (type1 == LIB_FS_DIR && type2 != LIB_FS_DIR) {
        return -1;
    }

    if (type2 == LIB_FS_DIR && type1 != LIB_FS_DIR) {
        return 1;
    }

    return 0;
}

int lib_file_compare_by_name(const char* name1, const char* name2) {
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

int lib_file_compare_by_size(uint64_t size1, uint64_t size2) {
    if (size2 == size1) {
        return 0;
    }
    return size1 > size2 ? 1 : -1; // 
    //return size1 > size2 ? -1 : 1; // ^   
}

int lib_file_compare_by_time(time_t time1, time_t time2) {
    if (time2 == time1) {
        return 0;
    }
    return time1 > time2 ? 1 : -1; // 
    //return time1 > time2 ? -1 : 1; // ^   
}

////

int lib_file_compare(const void* v1, const void* v2, lib_file_sort_t file_sort, bool asc, bool is_dir_first) {

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
        cmp = lib_file_compare_by_type(type1, type2);
        if (cmp != 0) {
            return cmp;
        }
    }

    cmp = 0;
    if (file_sort == LIB_FILE_SORT_BY_NAME) {

        /* Sort By name */
        char *name1 = f1->name;
        char *name2 = f2->name;
        cmp = lib_file_compare_by_name(name1, name2);
    } else if (file_sort == LIB_FILE_SORT_BY_SIZE) {

        /* Sort By size */
        uint64_t size1 = lib_file_get_size(f1);
        uint64_t size2 = lib_file_get_size(f2);
        cmp = lib_file_compare_by_size(size1, size2);
    } else if (file_sort == LIB_FILE_SORT_BY_TIME) {

        /* Sort By time */
        time_t time1 = lib_file_get_mtime(f1);
        time_t time2 = lib_file_get_mtime(f2);
        cmp = lib_file_compare_by_size(time1, time2);
    }

    return (asc || cmp == 0) ? cmp : (cmp * -1);
}

// file-sort (asc)

int lib_file_sort_by_alpha(const void* v1, const void* v2) {
    return lib_file_compare(v1, v2, LIB_FILE_SORT_BY_NAME, true, false);
}

int lib_file_sort_by_name(const void* v1, const void* v2) {
    return lib_file_compare(v1, v2, LIB_FILE_SORT_BY_NAME, true, true);
}

int lib_file_sort_by_size(const void* v1, const void* v2) {
    return lib_file_compare(v1, v2, LIB_FILE_SORT_BY_SIZE, true, false);
}

int lib_file_sort_by_time(const void* v1, const void* v2) {
    return lib_file_compare(v1, v2, LIB_FILE_SORT_BY_TIME, true, false);
}

// file-sort (desc)

int lib_file_sort_by_alpha_desc(const void* v1, const void* v2) {
    return lib_file_compare(v1, v2, LIB_FILE_SORT_BY_NAME, false, false);
}

int lib_file_sort_by_name_desc(const void* v1, const void* v2) {
    return lib_file_compare(v1, v2, LIB_FILE_SORT_BY_NAME, false, true);
}

int lib_file_sort_by_size_desc(const void* v1, const void* v2) {
    return lib_file_compare(v1, v2, LIB_FILE_SORT_BY_SIZE, false, false);
}

int lib_file_sort_by_time_desc(const void* v1, const void* v2) {
    return lib_file_compare(v1, v2, LIB_FILE_SORT_BY_TIME, false, false);
}
