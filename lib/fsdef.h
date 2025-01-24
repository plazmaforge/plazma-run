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

#endif // PLAZMA_LIB_FSDEF_H