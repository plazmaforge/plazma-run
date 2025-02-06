#ifndef PLAZMA_LIB_FSDEF_H
#define PLAZMA_LIB_FSDEF_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#include <wchar.h>
#endif

#define LIB_FS_DIR_SEPARATOR_NIX '/'
#define LIB_FS_DIR_SEPARATOR_STR_NIX "/"

#define LIB_FS_DIR_SEPARATOR_WIN '\\'
#define LIB_FS_DIR_SEPARATOR_STR_WIN "\\"

#define LIB_FS_IS_DIR_SEPARATOR_ALL(c) ((c) == LIB_FS_DIR_SEPARATOR_NIX || (c) == LIB_FS_DIR_SEPARATOR_WIN)

#ifdef _WIN32

/* Stuff missing in std vc6 api */
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES -1
#endif
#ifndef FILE_ATTRIBUTE_DEVICE
#define FILE_ATTRIBUTE_DEVICE 64
#endif

#define LIB_FS_DIR_SEPARATOR '\\'
#define LIB_FS_DIR_SEPARATOR_STR "\\"
#define LIB_FS_IS_DIR_SEPARATOR(c) ((c) == LIB_FS_DIR_SEPARATOR || (c) == '/')

#else

#define LIB_FS_DIR_SEPARATOR '/'
#define LIB_FS_DIR_SEPARATOR_STR "/"
#define LIB_FS_IS_DIR_SEPARATOR(c) ((c) == LIB_FS_DIR_SEPARATOR)

#endif

/*
 * File types
 */
#define LIB_FS_UNKNOWN       0
#define LIB_FS_FIFO          1
#define LIB_FS_CHR           2
#define LIB_FS_DIR           4
#define LIB_FS_BLK           6
#define LIB_FS_REG           8
#define LIB_FS_LNK          10
#define LIB_FS_SOCK         12
#define LIB_FS_WHT          14


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

#ifdef _WIN32

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


// errno

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


#endif // PLAZMA_LIB_FSDEF_H