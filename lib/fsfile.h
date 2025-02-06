#ifndef PLAZMA_LIB_FSFILE_H
#define PLAZMA_LIB_FSFILE_H

#include <sys/stat.h>

#include "fsdef.h"

// fs-stat

//#if (defined(__MINGW64_VERSION_MAJOR) || defined(_MSC_VER)) && !defined(_WIN64)
//typedef struct _stat32 lib_stat_t;
//typedef struct _stat32 stat;
//#elif defined(__MINGW64_VERSION_MAJOR) && defined(_WIN64)
//typedef struct _stat64 lib_stat_t;
//typedef struct _stat64 stat;
//#else
//typedef struct stat lib_stat_t;
//#endif

typedef struct stat lib_stat_t;

// fs-file

#ifdef _WIN32

//#include <windows.h>
//#include <errno.h>
//#include "strlib.h"
//#include "wstrlib.h"

/*
typedef struct _BY_HANDLE_FILE_INFORMATION {
  DWORD    dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD    dwVolumeSerialNumber;
  DWORD    nFileSizeHigh;
  DWORD    nFileSizeLow;
  DWORD    nNumberOfLinks;
  DWORD    nFileIndexHigh;
  DWORD    nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION;
*/

/* File Info (stat)   */
//typedef BY_HANDLE_FILE_INFORMATION lib_file_info_t;

/*
typedef struct _WIN32_FIND_DATAW {
  DWORD    dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD    nFileSizeHigh;
  DWORD    nFileSizeLow;
  DWORD    dwReserved0;
  DWORD    dwReserved1;
  WCHAR    cFileName[MAX_PATH];
  WCHAR    cAlternateFileName[14];
  DWORD    dwFileType;    // Obsolete. Do not use.
  DWORD    dwCreatorType; // Obsolete. Do not use
  WORD     wFinderFlags;  // Obsolete. Do not use
} WIN32_FIND_DATAW, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;
*/

#else

/*
typedef struct stat {
    ino_t       st_ino;         // File serial number.
    off_t       st_size;        // File size in bytes.
    dev_t       st_dev;         // ID of the device containing the file.
    dev_t       st_rdev;        // Device ID.
    uid_t       st_uid;         // User ID of file.
    gid_t       st_gid;         // Group ID of file.
    time_t      st_mtime;       // Time of last data modification.
    time_t      st_atime;       // Time when file data was last accessed.
    time_t      st_ctime;       // Time of last file status change.
    mode_t      st_mode;        // File types and permissions.
    nlink_t     st_nlink;       // Number of hard links to the file.
    blkcnt_t    st_blocks;      // Blocks allocated for a file.
    blksize_t   st_blksize;     // Preferred I/O block size for object.
}
*/

/* File Info          */
//#define lib_file_info_t struct stat;
//typedef struct stat lib_file_info_t;

#endif


//////////////////////////////////////////////////////////////////////////////////
////
//// File Size Max (int64_t) = 9,223,372,036,854,775,807 bytes = 8 EB (exabytes)
//// C (int64_t)             = Java (long)
////
//////////////////////////////////////////////////////////////////////////////////

typedef uint32_t fid_t;
typedef uint64_t fino_t;

typedef int64_t  fsize_t;
typedef int32_t  bsize_t;

typedef struct lib_file_info_t {
    int                  type;        // File type
    char*                path;        // File path

    fino_t               ino;         // File serial number.
    fsize_t /*uint32_t*/ size;        // File size in bytes.
    int     /*int32_t*/  dev;         // ID of the device containing the file.
    int     /*int32_t*/  rdev;        // Device ID.
    fid_t   /*uint32_t*/ uid;         // User ID of file.
    fid_t   /*uint32_t*/ gid;         // Group ID of file.
    time_t               mtime;       // Time of last data modification.
    time_t               atime;       // Time when file data was last accessed.
    time_t               ctime;       // Time of last file status change.
    int     /*uint16_t*/ mode;        // File types and permissions.
    int     /*uint16_t*/ nlink;       // Number of hard links to the file.
    fsize_t /*int64_t*/  blocks;      // Blocks allocated for a file.
    bsize_t /*int32_t*/  blksize;     // Preferred I/O block size for object.
} lib_file_info_t;

typedef struct lib_file_t {
  int type;
  char* name;
  lib_stat_t* stat;
} lib_file_t;

typedef struct lib_file_data_t {
    char* data;
    size_t size;
} lib_file_data_t;


#ifdef _WIN32

// errno

// static int to_errno_win(DWORD error_code) {
//   switch (error_code) {
//     case ERROR_ACCESS_DENIED:
//       return EACCES;
//       break;
//     case ERROR_ALREADY_EXISTS:
//     case ERROR_FILE_EXISTS:
//       return EEXIST;
//     case ERROR_FILE_NOT_FOUND:
//       return ENOENT;
//       break;
//     case ERROR_INVALID_FUNCTION:
//       return EFAULT;
//       break;
//     case ERROR_INVALID_HANDLE:
//       return EBADF;
//       break;
//     case ERROR_INVALID_PARAMETER:
//       return EINVAL;
//       break;
//     case ERROR_LOCK_VIOLATION:
//     case ERROR_SHARING_VIOLATION:
//       return EACCES;
//       break;
//     case ERROR_NOT_ENOUGH_MEMORY:
//     case ERROR_OUTOFMEMORY:
//       return ENOMEM;
//       break;
//     case ERROR_NOT_SAME_DEVICE:
//       return EXDEV;
//       break;
//     case ERROR_PATH_NOT_FOUND:
//       return ENOENT; /* or ELOOP, or ENAMETOOLONG */
//       break;
//     default:
//       return EIO;
//       break;
//     }
// }


#endif


#ifdef _WIN32

/* File descriptor    */
typedef HANDLE lib_fd_t;
#else

/* File descriptor    */
typedef int lib_fd_t;     

#endif

//// fs-stat

int lib_fs_stat(const char* path, lib_stat_t* buf);

#endif // PLAZMA_LIB_FSFILE_H
