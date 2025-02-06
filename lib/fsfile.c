#include "fsfile.h"
#include "strlib.h"

#ifdef _WIN32
#include <windows.h>
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

