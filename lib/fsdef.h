#ifndef PLAZMA_LIB_FSDEF_H
#define PLAZMA_LIB_FSDEF_H

#define LIB_FS_DIR_SEPARATOR_NIX '/'
#define LIB_FS_DIR_SEPARATOR_STR_NIX "/"

#define LIB_FS_DIR_SEPARATOR_WIN '\\'
#define LIB_FS_DIR_SEPARATOR_STR_WIN "\\"

#define LIB_FS_IS_DIR_SEPARATOR_ALL(c) ((c) == LIB_FS_DIR_SEPARATOR_NIX || (c) == LIB_FS_DIR_SEPARATOR_WIN)

#ifdef _WIN32

//#include <windows.h>

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

//#include <dirent.h>

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

#endif // PLAZMA_LIB_FSDEF_H