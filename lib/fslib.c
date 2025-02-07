#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <sys/utime.h>
#include "fslib_win.h"
#else
//#include <utime.h>
//#include <sys/unistd.h>
#include <unistd.h>  /* geuid, chdir, rmdir, access */

#include "fslib_nix.h"
#endif

#include "strlib.h"
#include "wclib.h"
#include "pathlib.h"
#include "fslib.h"

#define LIB_FS_BUF_SIZE 4096 // For file copy

//// fs-math ////

int lib_fs_match_file(const char* name, const char* pattern) {
    return lib_wc_match_file(name, pattern);
}

//// fs-check

bool lib_fs_exists(const char* path) {
    return lib_fs_file_check(path, LIB_FILE_CHECK_EXISTS);
}

bool lib_fs_is_reg(const char* path) {
    return lib_fs_file_check(path, LIB_FILE_CHECK_REG);
}

bool lib_fs_is_dir(const char* path) {
    return lib_fs_file_check(path, LIB_FILE_CHECK_DIR);
}

bool lib_fs_is_exec(const char* path) {
    return lib_fs_file_check(path, LIB_FILE_CHECK_EXEC);
}

//// fs-cmd ////

/* POSIX Style                */

int lib_fs_access(const char* path, int mode) {
#ifdef _WIN32
    wchar_t* wpath = lib_mbs_to_wcs(path);
    int retval;
    int save_errno;

    if (!wpath) {
        errno = EINVAL;
        return -1;
    }

#ifndef X_OK
#define X_OK 1
#endif

    retval = _waccess(wpath, mode & ~X_OK);
    save_errno = errno;
    free(wpath);
    errno = save_errno;
    return retval;
#else
    return access(path, mode);
#endif
}

int lib_fs_chmod(const char* path, int mode) {
#ifdef _WIN32
    wchar_t* wpath = lib_mbs_to_wcs(path);
    int retval;
    int save_errno;

    if (!wpath) {
        errno = EINVAL;
        return -1;
    }    

    retval = _wchmod(wpath, mode);
    save_errno = errno;
    free(wpath);
    errno = save_errno;
    return retval;
#else
    return chmod(path, mode);
#endif
}

int lib_fs_mkdir(const char* path, int mode) {
#ifdef _WIN32
    wchar_t* wpath = lib_mbs_to_wcs(path);
    int retval;
    int save_errno;

    if (!wpath) {
        errno = EINVAL;
        return -1;
    }

    retval = _wmkdir(wpath);
    save_errno = errno;
    free(wpath);
    errno = save_errno;
    return retval;
#else
    return mkdir(path, mode);
#endif
}

int lib_fs_chdir(const char* path) {
#ifdef _WIN32
    wchar_t* wpath = lib_mbs_to_wcs(path);
    int retval;
    int save_errno;

    if (!wpath) {
        errno = EINVAL;
        return -1;
    }

    retval = _wchdir(wpath);
    save_errno = errno;
    free(wpath);
    errno = save_errno;
    return retval;
#else
    return chdir(path);
#endif
}

int lib_fs_rename(const char* old_path, const char* new_path) {
#ifdef _WIN32
    wchar_t* wold_path;
    wchar_t* wnew_path;
    int retval;
    int save_errno = 0;

    wold_path = lib_mbs_to_wcs(old_path);
    if (!wold_path) {
        errno = EINVAL;
        return -1;
    }

    wnew_path = lib_mbs_to_wcs(new_path);
    if (!wnew_path) {
        free(wold_path);
        errno = EINVAL;
        return -1;
    }

    if (MoveFileExW(wold_path, wnew_path, MOVEFILE_REPLACE_EXISTING)) {
        retval = 0;
    } else {
        retval = -1;
        save_errno = to_errno_win(GetLastError());
    }

    free(wold_path);
    free(wnew_path);

    errno = save_errno;
    return retval;
#else
    return rename(old_path, new_path);
#endif
}

int lib_fs_move(const char* old_path, const char* new_path) {
    if (!old_path || !new_path) {
        errno = EINVAL;
        return -1;
    }

    /*
    If 'new_path' is a directory then
    we create new 'dst_file_name' = 'new_path' + 'base_name'
    where 'base_name' = 'get_base_name'(from 'old_path')
    */

    char* dst_file_name = (char*) new_path;
    bool is_dst_dir = false;
    if (lib_fs_is_dir(new_path)) {
        is_dst_dir = true;
        char* base_name = lib_fs_get_base_name(old_path);          // [allocate]
        dst_file_name = lib_fs_get_file_path(new_path, base_name); // [allocate]
        free(base_name);
    }

    int retval = lib_fs_rename(old_path, dst_file_name);
    if (is_dst_dir) {
        free(dst_file_name);
    }
    return retval;    
}

int lib_fs_copy(const char* src_file_name, const char* dst_file_name) {
    return lib_fs_copy_opts(src_file_name, dst_file_name, LIB_FS_BUF_SIZE);
}

int lib_fs_copy_opts(const char* src_file_name, const char* dst_file_name, int buf_size) {

    FILE* src_file = fopen(src_file_name, "rb");
    if (!src_file) {
        //strerror(errno)
        return -1;
    }

    FILE* dst_file = fopen(dst_file_name, "wb");
    if (!dst_file) {
        fclose(src_file);
        return -1;
    }

    int buffer[buf_size];
    int bytes;

    while ((bytes = fread(buffer, 1, buf_size, src_file)) != 0) {
        if (fwrite(buffer, 1, bytes, dst_file) != bytes) {
            fclose(src_file);
            fclose(dst_file);
            return -1;
        }
    }
    fclose(src_file);
    fclose(dst_file);
    return 0;    
}

int lib_fs_remove(const char* path) {
#ifdef _WIN32
    wchar_t* wpath = lib_mbs_to_wcs(path);
    int retval;
    int save_errno;

    if (!path) {
        errno = EINVAL;
        return -1;
    }

    retval = _wremove(wpath);
    if (retval == -1) {
        retval = _wrmdir(wpath);
    }
        
    save_errno = errno;
    free(wpath);
    errno = save_errno;    
    return retval;
#else
    return remove(path);
#endif
}

int lib_fs_rmdir(const char* path) {
#ifdef _WIN32
    wchar_t* wpath = lib_mbs_to_wcs(path);
    int retval;
    int save_errno;

    if (!wpath) {
        errno = EINVAL;
        return -1;
    }

    retval = _wrmdir(wpath);
    save_errno = errno;
    free(wpath);
    errno = save_errno;
    return retval;
#else
    return rmdir(path);
#endif
}

bool lib_fs_file_check(const char* file_name, lib_file_check_t check) {
    if (!file_name) {
        return 0;
    }

#ifdef _WIN32
    DWORD attributes;
    wchar_t *wfile_name;

    wfile_name = lib_mbs_to_wcs(file_name);
    if (!wfile_name)
        return 0;

    attributes = GetFileAttributesW(wfile_name);
    free(wfile_name);

    if (attributes == INVALID_FILE_ATTRIBUTES)
        return 0;

    if (check & LIB_FILE_CHECK_EXISTS)
        return 1;

    if (check & LIB_FILE_CHECK_LNK) {
        if (attributes & FILE_ATTRIBUTE_REPARSE_POINT)
            return 1;
    }

    if (check & LIB_FILE_CHECK_REG) {
        if (!(attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE)))
            return 1;
    }

    if (check & LIB_FILE_CHECK_DIR) {
        if (attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE))
            return 1;
    }

    if (check & LIB_FILE_CHECK_EXEC) {
        return lib_fs_is_exec_ext(file_name);
    }

    return 0;
#else
    if ((check & LIB_FILE_CHECK_EXISTS) && (access(file_name, F_OK) == 0))
        return 1;

    if ((check & LIB_FILE_CHECK_EXEC) && (access(file_name, X_OK) == 0)) {
        if (getuid() != 0)
            return 1;

        /* For root, on some POSIX systems, access (filename, X_OK)
         * will succeed even if no executable bits are set on the
         * file. We fall through to a stat test to avoid that.
         */
    } else {
        int _check = check;
        _check &= ~LIB_FILE_CHECK_EXEC;
        check = (lib_file_check_t) _check;
    }

    if (check & LIB_FILE_CHECK_LNK) {
        struct stat s;

        if ((lstat(file_name, &s) == 0) && S_ISLNK(s.st_mode))
            return 1;
    }

    if (check & (LIB_FILE_CHECK_REG |
                 LIB_FILE_CHECK_DIR |
                 LIB_FILE_CHECK_EXEC)) {
                    
        struct stat s;

        if (stat(file_name, &s) == 0) {

            if ((check & LIB_FILE_CHECK_REG) && S_ISREG(s.st_mode))
                return 1;

            if ((check & LIB_FILE_CHECK_DIR) && S_ISDIR(s.st_mode))
                return 1;

            /* The extra test for root when access (file, X_OK) succeeds.
             */
            if ((check & LIB_FILE_CHECK_EXEC) &&
                ((s.st_mode & S_IXOTH) ||
                 (s.st_mode & S_IXUSR) ||
                 (s.st_mode & S_IXGRP)))
                return 1;
        }
    }

    return 0;
#endif
}

////////////////////////////////

int lib_fs_mkdir_all(const char* path, int mode) {
    char* fn;
    char* p;

    if (lib_stremp(path)) {
        errno = EINVAL;
        return -1;
    }

    /* try to create the full path first */
    if (lib_fs_mkdir(path, mode) == 0) {
        return 0;
    }

    if (errno == EEXIST) {
        if (!lib_fs_file_check(path, LIB_FILE_CHECK_DIR)) {
           errno = ENOTDIR;
           return -1;
        }
        return 0;
    }

    /* walk the full path and try creating each element */
    fn = lib_strdup(path);

    if (lib_fs_is_absolute_path(fn))
        p = (char*) lib_fs_skip_root(fn);
    else
        p = fn;

    do {

        /* skip non-dir separators: search first dir separator */
        while (*p && !LIB_FS_IS_DIR_SEPARATOR(*p))
            p++;

        /* replace dir separartor to NULL-terminal char */
        if (!*p)
            p = NULL;
        else
            *p = '\0';

        if (!lib_fs_file_check(fn, LIB_FILE_CHECK_EXISTS)) {
            if (lib_fs_mkdir(fn, mode) == -1 && errno != EEXIST) {
                int save_errno = errno;
                if (errno != ENOENT || !p) {
                    free(fn);
                    errno = save_errno;
                    return -1;
                }
            }
        } else if (!lib_fs_file_check(fn, LIB_FILE_CHECK_DIR)) {
           free(fn);
           errno = ENOTDIR;
           return -1;
        }

        /* restore dir separator */
        if (p) {
            *p++ = LIB_FS_DIR_SEPARATOR;

            /* skip dir separators: search first non-dir separator char*/
            while (*p && LIB_FS_IS_DIR_SEPARATOR(*p))
                p++;
        }

    } while (p);

    free(fn);

    return 0;
}

/* Human Style                */

int lib_fs_create_dir(const char* path) {
    return lib_fs_mkdir(path, 0775);
}

int lib_fs_create_dir_all(const char* path) {
    return lib_fs_mkdir_all(path, 0775);
}

int lib_fs_remove_file(const char* path) {
    return lib_fs_remove(path);
}

int lib_fs_remove_dir(const char* path) {
    return lib_fs_rmdir(path);
}

//

// file-scan

int lib_fs_is_recursive_ignore(const char* path) {
    if (!path) {
        return 1;
    }
    return (strcmp(path, ".") == 0 || strcmp(path, "..") == 0);
}

int lib_fs_is_classic_ignore(const char* path) {
    if (!path || path[0] == '\0') {
        return 1;
    }
    // starts with '.': .git, .svn
    return path[0] == '.';
}

int lib_fs_is_ignore(const char* path) {
    return 0; //return lib_fs_is_recursive_ignore(path) || lib_fs_is_classic_ignore(path);
}

////

int lib_fs_scandir_internal(const char* dir_name, /*const*/ char** patterns, int pattern_count, lib_file_t*** files, int* file_count, int level, int max_depth, int file_only) {

    DIR* dir = lib_opendir(dir_name);
    if (!dir) {
        return -1;
    }

    //errno = 0;
    struct dirent* dirent;
    const char* pattern = NULL;

    // BUG (!) 
    if (patterns && level >= 0)  {
        if (level > pattern_count - 1) {
            if (pattern_count == 1) {
                // patterns: '*' or '*.*'
                pattern = patterns[0];
            }
        } else {
            pattern = patterns[level];
        }
    } 

    bool use_stat = true;          // optional
    bool use_dir = !file_only;     // optional
    bool use_force_pattern = true; // optional
    bool is_force_pattern = use_force_pattern && pattern && pattern_count == 1;

    while ((dirent = lib_readdir(dir)) != NULL) {

        char* file_name = dirent->d_name;
        int file_type = dirent->d_type; // lib_fs_get_dirent_type(dirent);                       // dirent type (!)
        int is_recursive_ignore = lib_fs_is_recursive_ignore(file_name); // ., ..
        int is_ignore = lib_fs_is_ignore(file_name);                     // .git, .svn
        int is_force_ignore = is_recursive_ignore || is_ignore;
        int is_match = is_ignore ? 0 : (pattern == NULL || lib_fs_match_file_internal(pattern, file_name));
        int is_dir_ = dirent->d_type == LIB_FS_DIR; // lib_fs_is_dirent_dir(dirent);                           // dirent type (!)

        if (!is_match && is_dir_ && is_force_pattern && !is_force_ignore) {
            is_match = 1;
        }

        //printf("find [%s] [%s%s] [%d] %s, %s, %s\n", (is_match ? "+" : " "), (is_dir_ ? "D" : " "), (is_ignore ? "I" : " "), level, dir_name, file_name, pattern);

        if (is_match) {

            //printf("match [%d] %s, %s, %s\n", level, dir_name, file_name, pattern);

            int mode = 0; // 0 - notning, 1 - add file/dir, 2 - scandir recursive
            if (!is_dir_) {
                // We add the file from last pattern level only
                if (level == 0 || level == pattern_count - 1 || is_force_pattern) {
                    mode = 1; // add file
                }
            } else {
                // Recursive if max_depth != -1
                if (max_depth >= 0 && !is_recursive_ignore) {
                    mode = 2;
                }
                if (mode == 0 && use_dir) {
                    mode = 1; // add dir
                }
            }

            if (mode == 0) {
                continue; // notning
            }

            char* full_name = lib_fs_get_normalize_path(dir_name, file_name);

            //printf("match: full_name    : %s\n", full_name);

            if (mode == 1) {

                //printf("try   : add_file\n");
                int index = *file_count; // old file_count
                lib_file_t** list = *files;

                if (list[index] == NULL) { // NULL-terminate array: +1
                    const int inc = 10;	/* increase by this much */
                    int size = index + inc;
                    if (lib_files_reinit(files, size) != 0) {
                        free(full_name);
                        lib_files_free(*files);
                        lib_closedir(dir);
                        return -1;
                    }
                }

                lib_file_t* file_s = lib_file_new();
                file_s->name = strdup(full_name);
                file_s->type = file_type;                

                if (use_stat) {
                    char* real_path = lib_fs_get_real_path(file_s->name);
                    lib_stat_t* stat = lib_fs_stat_new();
                    lib_fs_stat(real_path, stat);
                    file_s->stat = stat;
                    free(real_path);
                }

                //printf("try  : index        : %d\n", *file_count);
                //printf("try  : reserved     : %d\n", *reserved);

                list = *files;
                list[index] = file_s; 
                //*files[*file_count] = file_s; // Doesn't work

                *file_count = *file_count + 1;
                 
                // Shift NULL marker (size + 1): for allocation without fulling NULL
                index = *file_count; // new file_count
                if (list[index] != NULL) {
                    list[index] = NULL;
                }

                //printf("try  : file_count   : %d\n", *file_count);

            }
                
            if (mode == 2) {
               //printf("try   : add_dir\n");

               int result = lib_fs_scandir_internal(full_name, patterns, pattern_count, files, file_count, level + 1, max_depth, file_only);
               if (result < 0) {
                 //printf("err  : %d\n", result);
                 // error
               }
            }

            free(full_name);
        }
    }
    //if (errno != 0) {
        // TODO: stderr: error
    //}

    lib_closedir(dir);
    return 0;
}

// file-scan

int lib_fs_scandir(const char* dir_name, const char* pattern, lib_file_t*** files, int max_depth, int file_only) {
    if (!dir_name) {
        return -1;
    }

    char** patterns = lib_path_split(pattern); // split pattern by level
    int pattern_count = lib_strarrlen(patterns);

    //printf(">>pattern_count : %d\n", pattern_count);
    //printf(">>scandir       : %s\n", dir_name);
    //printf(">>level         : %d\n", level);
    //printf(">>pattern       : %s\n", pattern);

    int file_count = 0;
    int size = 10; // start size

    lib_files_init(files, size);

    lib_fs_scandir_internal(dir_name, /*(const char**)*/ patterns, pattern_count, files, &file_count, 0, max_depth, file_only);

    lib_strarrfree(patterns);

    if (files && file_count > 0) {
        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_file_sort_by_alpha);
        qsort(*files, file_count, sizeof(struct lib_file_t*), lib_file_sort_by_name);
        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_file_sort_by_size);
        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_file_sort_by_time);

        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_file_sort_by_alpha_desc);
        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_file_sort_by_name_desc);
        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_file_sort_by_size_desc);
        //qsort(*files, file_count, sizeof(struct lib_file_t*), lib_file_sort_by_time_desc);

    }
    
    return file_count;
}

// https://github.com/tronkko/dirent/tree/master
// https://github.com/tronkko/dirent/blob/master/examples/scandir.c

// https://github.com/Quintus/pathie-cpp/blob/master/src/path.cpp