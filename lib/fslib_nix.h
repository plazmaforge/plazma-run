#ifndef PLAZMA_LIB_FSLIB_NIX_H
#define PLAZMA_LIB_FSLIB_NIX_H

#if defined __unix__ || defined __linux__ || (defined __APPLE__ && defined __MACH__)

#include <errno.h>

#include <fnmatch.h>
#include <pwd.h>       /* getpwuid: passwd    */
#include <grp.h>       /* getgrgid: group     */
#include <sys/xattr.h> /* listxattr           */
#include <sys/acl.h>   /* acl_get_file: acl_t */

#include "strlib.h"
#include "fslib.h"

static int lib_fs_match_file_internal(const char* pattern, const char* name);

static int lib_fs_match_file_internal_mode(const char* pattern, const char* name, int mode);

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