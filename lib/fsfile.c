#include "fsfile.h"
#include "strlib.h"

#ifdef _WIN32
#include <windows.h>
#include "wstrlib.h"
#else
#endif

#ifdef _WIN32

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

