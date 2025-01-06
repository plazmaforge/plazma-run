#ifndef PLAZMA_LIB_FSLIB_NIX_H
#define PLAZMA_LIB_FSLIB_NIX_H

#if defined __unix__ || defined __linux__ || (defined __APPLE__ && defined __MACH__)

#include <errno.h>
#include <dirent.h>
#include <fnmatch.h>
#include <limits.h>    /* PATH_MAX            */
#include <pwd.h>       /* getpwuid: passwd    */
#include <grp.h>       /* getgrgid: group     */
#include <sys/xattr.h> /* listxattr           */
#include <sys/acl.h>   /* acl_get_file: acl_t */

#include "strlib.h"
#include "fslib.h"

static int lib_fs_match_file_internal(const char* pattern, const char* name);

static int lib_fs_match_file_internal_mode(const char* pattern, const char* name, int mode);

// [allocate]
char* lib_fs_get_normalize_path(const char* dir_name, const char* file_name) {
    return lib_fs_get_file_path(dir_name, file_name); // nothing to do 
}

// [allocate]
char* lib_fs_get_real_path(const char* path) {
    if (!path) {
        return NULL;
    }
    char buf[PATH_MAX];
    if (realpath(path, buf) != buf) {
        return NULL;
    }
    return strdup(buf);
}

// [allocate]
char* lib_fs_get_current_dir() {
  /* Current directory */
  char buf[PATH_MAX];
  //errno = 0;
  if (getcwd(buf, sizeof(buf)) == NULL) {
       return NULL;
  } else {
       return strdup(buf);
  }
  return NULL;
}

const char* lib_fs_get_current_find_path() {
    return ".";
}

int lib_fs_is_current_find_path(const char* path) {
    if (!path) {
        return 0;
    }
    return path[0] == '.';
}

////

static int _lib_fs_is_dir(struct dirent* file) {
    if (file == NULL) {
        return false;
    }
    return file->d_type == DT_DIR;
}

static void _lib_fs_normalize_slash(char* path, size_t len) {
    lib_strntr(path, len, '\\', '/');
}

// void scandir_internal(const char* dirName, const char* pattern, std::vector<std::string>& files, int level, int max_depth, int total_level, char* level_pattern) {

//     struct dirent* file;
//     DIR* dir = opendir(dirName);
//     if (dir == NULL) {
//         fprintf(stderr, "Directory not found: %s\n", dirName);
//         return;
//     }

//     errno = 0;
//     while ((file = readdir(dir)) != NULL) {

//         char* fileName = file->d_name;

//         //printf("try [%d] %s, %s, :: %s\n", level, dirName, fileName, level_pattern);
//         if (pattern == NULL || fs_match_file_internal(level_pattern, fileName)) {

//             int mode = 0; // 0 - notning, 1 - file, 2 - dir
//             if (!_fs_is_dir(file)) {
//                 // We add the file from last pattern level only
//                 mode = (level == 0 || level == total_level - 1) ? 1 : 0;
//             } else {
//                 // Recursive if max_depth != -1
//                 mode = max_depth >= 0 ? 2 : 0;
//             }

//             if (mode == 0) {
//                 continue; // notning
//             }

//             char* fullName = fs_get_file_path(dirName, fileName);

//             //printf("match:fullName: %s\n", fullName);
//             //printf("match: [%s] %s, %s, %s\n", (mode == 2 ? "D" : " "), fullName, dirName, fileName);

//             if (mode == 1) {
//                 files.push_back(fullName);
//             } else if (mode == 2) {
//                 scandir(fullName, pattern, files, level + 1);
//             }

//             free(fullName);
//         }
//     }
//     if (errno != 0) {
//         // TODO: stderr: error
//     }
//     closedir(dir);
// }

////

int lib_fs_is_dirent_dir(lib_fs_dirent_t* dirent) {
    if (!dirent) {
        return 0;
    }
    return dirent->fd->d_type == DT_DIR;
}

int lib_fs_get_dirent_type(lib_fs_dirent_t* dirent) {
    if (!dirent) {
        return 0;
    }
    return dirent->fd->d_type;
}

// [allocate]
lib_fs_dir_t* lib_fs_open_dir(const char* dir_name) {
    if (!dir_name) {
        return NULL;
    }
    DIR* _dir = opendir(dir_name);
    if (!_dir) {
        fprintf(stderr, "Directory not found: %s\n", dir_name);
        return NULL;
    }
    lib_fs_dir_t* dir = (lib_fs_dir_t*) malloc(sizeof(lib_fs_dir_t));
    if (!dir) {
        closedir(_dir);
        return NULL;
    }
    dir->ptr = _dir;
    dir->dirent = NULL;
    return dir;
}

// [allocate]
lib_fs_dirent_t* lib_fs_read_dir(lib_fs_dir_t* dir) {
    if (!dir) {
        return NULL;
    }
    struct dirent* fd = readdir(dir->ptr);
    if (!fd) {
        return NULL;
    }
    if (!dir->dirent) {
        dir->dirent = (lib_fs_dirent_t*) malloc(sizeof(lib_fs_dirent_t));
        dir->dirent->type = 0;
        dir->dirent->name = NULL;
    }
    if (!dir->dirent) {
        return NULL;
    }
    dir->dirent->fd = fd;
    dir->dirent->type = lib_fs_get_dirent_type(dir->dirent);
    dir->dirent->name = fd->d_name;
    return dir->dirent;
}

int lib_fs_close_dir(lib_fs_dir_t* dir) {
    if (!dir) {
        return 0;
    }
    int result = closedir(dir->ptr);
    // free(dir->dirent) // ???
    dir->dirent = NULL;
    free(dir);
    return result;
}

////

static int lib_fs_match_file_internal(const char* pattern, const char* name) {
    return lib_fs_match_file_internal_mode(pattern, name, FNM_PERIOD);
}

static int lib_fs_match_file_internal_mode(const char* pattern, const char* name, int mode) {
    //printf(" %s -> %s, %d, %d\n", pattern, name, val, res);
    //return fnmatch(pattern, name, FNM_PERIOD) == 0; // true id '0'

    int val = fnmatch(pattern, name, mode);
    int res = val == 0;
    return res;

    //return match(name, pattern); // rotate pattern, name !
}

//// uname, gname

static char* lib_fs_get_uname_by_id(uid_t uid) {
    struct passwd* pws;
    pws = getpwuid(uid);
    return pws->pw_name;
}

static char* lib_fs_get_gname_by_id(gid_t gid) {
    struct group* g = getgrgid(gid);
    return g ? g->gr_name : NULL;
}

char* lib_fs_file_get_uname(lib_fs_file_t* file) {
    if (!file || !file->stat) {
        return NULL;
    }
    return lib_fs_get_uname_by_id(file->stat->st_uid);
}

char* lib_fs_file_get_gname(lib_fs_file_t* file) {    
    if (!file || !file->stat) {
        return NULL;
    }
    return lib_fs_get_gname_by_id(file->stat->st_gid);
}

char lib_fs_get_mode_access(const char* path) {
    if (!path) {
        return ' ';
    }
    acl_t acl = NULL;
    acl_entry_t dummy;
    ssize_t xattr = 0;
    char chr;

    acl = acl_get_link_np(path, ACL_TYPE_EXTENDED);
    if (acl && acl_get_entry(acl, ACL_FIRST_ENTRY, &dummy) == -1) {
        acl_free(acl);
        acl = NULL;
    }
    xattr = listxattr(path, NULL, 0, XATTR_NOFOLLOW);
    if (xattr < 0) {
        xattr = 0;
    }
        
    if (xattr > 0) {
        chr = '@';
    } else if (acl != NULL) {
        chr = '+';
    } else {
        chr = ' ';
    }

    if (acl) {
        acl_free(acl);
    }
    //printf("%c\n", chr);
    return chr;
}

#endif

#endif // PLAZMA_LIB_FSLIB_NIX_H