#ifndef PLAZMA_LIB_FSDIRENT_H
#define PLAZMA_LIB_FSDIRENT_H

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

#include "fsdef.h"

#ifdef _WIN32

/* Directory pointer  */
//typedef HANDLE lib_fs_dir_t;

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

/* Directory entry    */
typedef struct lib_fs_dirent_t {
    int type; // OS Indepentent
    char* name;
    WIN32_FIND_DATAW fd;
} lib_fs_dirent_t;

typedef struct lib_fs_dir_t {
    void* ptr;
    lib_fs_dirent_t* dirent;
} lib_fs_dir_t;


//////////////////////////////////////////////////////////////////////////

/* Maximum length of file name */
#if !defined(PATH_MAX)
#define PATH_MAX MAX_PATH
#endif

struct dirent  {
    uint64_t  d_ino;            /* File number of entry */
    uint64_t  d_off;            /* Seek offset (optional, used by servers) */
    uint16_t  d_reclen;         /* Length of this record */
    uint16_t  d_namlen;         /* Length of string in d_name */
    uint8_t   d_type;           /* File type, see below */
    //char      d_name[PATH_MAX]; /* Entry name (up to PATH_MAX bytes) */
    char*      d_name;
};

typedef struct DIR {    
    HANDLE handle;
    WIN32_FIND_DATAW data;
    wchar_t* wpath;
	struct dirent ent;
} DIR;

#else

/*
struct dirent {
    ino_t          d_ino;       // Inode number
    off_t          d_off;       // Offset to the next dirent
    unsigned short d_reclen;    // Length of this record
    unsigned char  d_type;      // Type of file; not supported by all file system types
    char           d_name[NAME_MAX]; // filename
};
*/

/* Directory entry    */
typedef struct lib_fs_dirent_t {
    int type; // OS Indepentent
    char* name;
    struct dirent* fd;
} lib_fs_dirent_t;

typedef struct lib_fs_dir_t {
    DIR* ptr;
    lib_fs_dirent_t* dirent;
} lib_fs_dir_t;

#endif

#ifdef _WIN32
#else
#endif

//// fs-psx

struct dirent* lib_readdir(DIR* dir);

DIR* lib_opendir(const char *dirname);

int lib_closedir(DIR* dir);

////////////////////////////////////////////////////////////

//// fs-dirent

bool lib_fs_is_dirent_dir(lib_fs_dirent_t* dirent);

int lib_fs_get_dirent_type(lib_fs_dirent_t* dirent);

// [allocate]
lib_fs_dir_t* lib_fs_open_dir(const char* dir_name);

lib_fs_dirent_t* lib_fs_read_dir(lib_fs_dir_t* dir);

int lib_fs_close_dir(lib_fs_dir_t* dir);

#endif // PLAZMA_LIB_FSDIRENT_H