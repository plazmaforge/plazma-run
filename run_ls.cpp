// Windows: Important lib order for UTF-8 (65001)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <grp.h>
#include <pwd.h>
#endif

#include "getopt.h"
#include "fslib.h"
#include "iodef.h"
#include "fmtlib.h"
#include "strlib.h"

void usage() {
    fprintf(stderr, "Usage: run-ls\n");
}

int _lib_len_counter(int val) {
    if (val == 0) {
        return 0;
    }
    long num = val;
    int len = 0;
    if(val < 0) {
        num *= -1;
    }        
    while(num != 0) {
        num = num / 10;
        len++;
    }
    return len;
}

char* _lib_itoa(int val) {
    if(val == 0) {
        return lib_strdup("0");
    }
        
    char* ret = lib_strnew(100);
    int len = _lib_len_counter(val);

    if (val < 0) {
        len++;
        ret[0] = '-';
        val *= -1;
    }

    while (val != 0) {
        ret[len - 1] = val % 10 + 48;
        val = val / 10;
        len--;
    }
    return ret;
}

////

#ifdef _WIN32

static char* _lib_wcs_to_mbs_win(UINT cp, const wchar_t* wstr, int wlen) {
    if (!wstr) {
        return NULL;
    }
    int len = WideCharToMultiByte(cp, 0, wstr, wlen, NULL, 0, NULL, NULL);
    char* str = lib_strnew(len);
    WideCharToMultiByte(cp, 0, wstr, -1, str, len, NULL, NULL);
    str[len] = '\0';
    return str;
}

/**
 * [allocate]
 */
char* _lib_wcs_to_mbs_n(const wchar_t* wstr, int wlen) {
    if (!wstr) {
        return NULL;
    }
    //#ifdef _WIN32
    return _lib_wcs_to_mbs_win(CP_UTF8, wstr, wlen);
    //#else
    // Locale dependency! - setlocale(LC_ALL, "");    
    //return lib_wcs_to_mbs_nix(wstr, wlen);
    //#endif
}

/**
 * [allocate]
 */
char* _lib_wcs_to_mbs(const wchar_t* wstr) {
    if (!wstr) {
        return NULL;
    }
    return _lib_wcs_to_mbs_n(wstr, wcslen(wstr)); 
}

wchar_t* getUserNameW() {
   WCHAR *uname = _wgetenv(L"USERNAME");
   if (uname != NULL && wcslen(uname) > 0) {
      return _wcsdup(uname);
   } 
   
   DWORD buflen = 0;
   if (GetUserNameW(NULL, &buflen) == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
      uname = (WCHAR*) malloc(buflen * sizeof(WCHAR));
      if (uname != NULL && GetUserNameW(uname, &buflen) == 0) {
        free(uname);
        uname = NULL;
      }
   } else {
        uname = NULL;
   }

   if (uname != NULL) {
      return _wcsdup(uname);
   }

   return NULL;  
}

char* getUserName() {
    wchar_t* wusername = getUserNameW();
    if (wusername == NULL) {
        return NULL;
    }
    char* username = _lib_wcs_to_mbs(wusername);
    if (username == NULL) {
        return NULL;
    }
    free(wusername);
    return username;
}

#else

////

bool _is_heap(void* ptr) {
    int x;

    uint64_t* pi = (uint64_t*) ptr;
    uint64_t* px = (uint64_t*) &x;

    return (pi < px);
}

////
char* _get_uname_by_id(uid_t uid) {
    struct passwd* pws;
    pws = getpwuid(uid);
    return pws->pw_name;
}

char* _get_gname_by_id(gid_t gid) {
    struct group* g = getgrgid(gid);
    return g ? g->gr_name : NULL;
}

#endif

////

static char* _uname = NULL;
static char* _gname = NULL;

char* _get_uname(lib_fs_file_t* file) {
    #ifdef _WIN32
    if (_uname) {
        return _uname;
    }
    _uname = getUserName();
    if (!_uname) {
        _uname = lib_strdup("");
    }
    return _uname;
    #else
    if (!file || !file->stat) {
        return NULL;
    }
    return _get_uname_by_id(file->stat->st_uid);
    #endif
}

char* _get_gname(lib_fs_file_t* file) {    
    #ifdef _WIN32
    if (_gname) {
        return _gname;
    }
    _gname = "197121"; // TODO: Stub
    //if (!_gname) {
    //    _gname = lib_strdup("");
    //}
    return _gname;
    #else
    if (!file || !file->stat) {
        return NULL;
    }
    return _get_gname_by_id(file->stat->st_gid);
    #endif

}

char* _get_gid_str(lib_fs_file_t* file) {
    if (!file || !file->stat) {
        return NULL;
    }
    return _lib_itoa(file->stat->st_gid);
}

int main(int argc, char *argv[]) {

    bool error = false;
    int opt;
    while ((opt = getopt(argc, argv, "al")) != -1) {
        switch (opt) {
        case 'a':
            // TODO
            break;
        case 'l':
            // TODO
            break;
        case '?':
            error = true;
            break;
        }
    }

    char* dir_name = lib_fs_get_current_dir(); //fs_get_current_find_path();
    //printf("Current Dir: %s\n", dir_name);

    char* pattern = NULL;
    lib_fs_file_t** files = NULL;
    lib_fs_file_t* file = NULL;
    int file_count = lib_fs_scandir(dir_name, pattern, &files, LIB_FS_SCANDIR_FLAT, false);

    if (file_count <= 0) {
        lib_fs_files_free(files);
        free(dir_name);
        return 0;
    }

    //lib_io_buf_init();

    bool use_date = true;
    bool use_time = true;
    bool use_size = true;
    bool use_size_first = true;

    int DATE_LEN = 10; // YYYY-MM-DD
    int TIME_LEN = 8;  // HH:MM:SS
    int BUF_LEN = DATE_LEN + 1 + (use_time ? (TIME_LEN + 1) : 0);
    char buf[BUF_LEN];

    int pos = 0;
    int stat_pos = -1;
    int total = 0;

    size_t buf_size = 0; //lib_io_stdout_get_buf_size();
    //buf_size = lib_io_get_buf_page(buf_size);

    //printf("buf size: %lu\n", buf_size);
    //fflush(stdout);
    //int max = 0;

    //int err = 0;
    //if ((err = fflush(stdout)) != 0) {
    //   fprintf(stderr, "error-1: %d\n", err);
    //   perror("fflush error\n");
    //}

    struct file_entry_t {
        lib_fs_file_t* file;
        char* path;
        char* name;
        //char* nlink;
        int nlink;
        char* uname;
        char* gname;
        char* gid;
        //
        int nlink_len;
        int name_len;
        int uname_len;
        int gname_len;
    };

    file_entry_t* entry = NULL;
    file_entry_t* entries[file_count];

    int max_nlink_len = 0;
    int max_name_len = 0;
    int max_uname_len = 0;
    int max_gname_len = 0;

    for (int i = 0; i < file_count; i++) {
        file = files[i];
        entry = (file_entry_t*) malloc(sizeof(file_entry_t));

        // char k[3];
        // int x;
        // int y;

        // uint64_t* pe = (uint64_t*) entry;
        // uint64_t* pk = (uint64_t*) k;
        // uint64_t* px = (uint64_t*) &x;

        // printf("entry*: %p\n", entry);
        // printf("k*    : %p\n", k);
        // printf("x*    : %p\n", &x);
        // printf("y*    : %p\n", &y);
        // printf("\n");
        // printf("entry*: %s\n", (pe < px) ? "heap" : "stack");
        // printf("k*    : %s\n", (pk < px) ? "heap" : "stack");         
        // printf("\n");
        // printf("entry*: %s\n", is_heap(pe) ? "heap" : "stack");
        // printf("k*    : %s\n", is_heap(pk) ? "heap" : "stack");         
        // printf("\n");
        // printf("pe*   : %p\n", pe);
        // printf("pk*   : %p\n", pk);
        // printf("px*   : %p\n", px);

        entries[i] = entry;

        if (!file) {
            continue;
        }

        entry->file = file;
        entry->path = file->name;
        entry->name = lib_fs_get_base_name(entry->path); // allocate
        entry->nlink = file->stat->st_nlink; //_lib_itoa(file->stat->st_nlink);  // allocate
        entry->uname = _get_uname(file);
        entry->gname = _get_gname(file);
        entry->gid = NULL;

        if (!entry->gname) {
            entry->gid = _get_gid_str(file);             // allocate
            entry->gname = entry->gid;
        }

        entry->name_len = entry->name ? strlen(entry->name) : 0;
        //entry->nlink_len = entry->nlink ? strlen(entry->nlink) : 0;
        entry->nlink_len = _lib_len_counter(entry->nlink);
        entry->uname_len = entry->uname ? strlen(entry->uname) : 0;
        entry->gname_len = entry->gname ? strlen(entry->gname) : 0;

        if (entry->name_len > max_name_len) {
            max_name_len = entry->name_len;
        }
        if (entry->nlink_len > max_nlink_len) {
            max_nlink_len = entry->nlink_len;
        }
        if (entry->uname_len > max_uname_len) {
            max_uname_len = entry->uname_len;
        }
        if (entry->gname_len > max_gname_len) {
            max_gname_len = entry->gname_len;
        }
    }

    printf("max_name_len  : %d\n", max_name_len);
    printf("max_nlink_len : %d\n", max_nlink_len);
    printf("max_uname_len : %d\n", max_uname_len);
    printf("max_gname_len : %d\n\n", max_gname_len);

    for (int i = 0; i < file_count; i++) {

        entry = entries[i];
        file = entry->file; // files[i];

        if (!file) {
            continue;
        }

        pos = 0;
        char* name = entry->name;       //lib_fs_get_base_name(path);
        int name_len = entry->name_len; // strlen(name);

        if (buf_size > 0 && i > 0) {
            int try_pos = (stat_pos > 0 ? stat_pos : 0) + name_len;
            if (total + try_pos > buf_size) {
                fflush(stdout);
                //total = 0;
                //pos = printf("%s: %d, %d, %d, %d\n", "FLUSH!!", total, try_pos, stat_pos, name_len);
                total = 0;
            }
        }

        /* Print Mode      */
        char mode[10 + 1];
        memset(mode, '-', 10);
        mode[10] = '\0';
        mode[0] = lib_fs_file_get_file_type_char(file);
        lib_fs_file_mode_add(file, mode);
        pos += printf("%s  ", mode);
        //pos++;
        
        /* Print NLink      */
        //char* nlink = entry->nlink;       // _itoa(file->stat->st_nlink);
        int nlink = entry->nlink;       // _itoa(file->stat->st_nlink);
        //pos += printf("%*s ", max_nlink_len, nlink);
        pos += printf("%*d ", max_nlink_len, nlink);
        //if (nlink) {
        //    free(nlink);
        //}

        /* Print User Name  */
        char* uname = entry->uname;       //_get_uname(file);
        pos += printf("%-*s  ", max_uname_len, uname);

        /* Print Group Name */
        char* gname = entry->gname;       // _get_gname(file);
        char* gid = entry->gid;           // NULL;
        pos += printf("%-*s ", max_gname_len, gname);
        if (!gid) {
            free(gid);
        }
        //pos++;
        
        /* Print Size    */
        if (use_size & use_size_first) {
            uint64_t size = lib_fs_file_get_file_size(file);
            pos += lib_fmt_print_file_size(size);
            //pos++;
        }

        /* Print DateTime */
        if (use_date) {
            time_t time = lib_fs_file_get_file_mtime(file);
            pos += lib_fmt_print_file_date_time(time, buf, BUF_LEN, use_time);
            //pos++;
        }

        /* Print Size    */
        if (use_size & !use_size_first) {
            uint64_t size = lib_fs_file_get_file_size(file);
            pos += lib_fmt_print_file_size(size);
            //pos++;
        }

        if (stat_pos < 0) {
            stat_pos = pos;
            if (stat_pos < 0) {
                stat_pos = 0;
            }
        }

        /* Print Name   */
        pos += printf("%s\n", name);
        total += pos;
        //if (pos > max) {
        //    max = pos;
        //}

        free(name);
        free(entry);
    }

    //printf("\n\nmax: %i\n", max);

    ////

    /*
    fflush(stdout);
    int num = 0;
    int maxcur = 0;
    for (int i = 0; i < 10000; i++) {
        int cur = 0;
        int tot = 0;
        if (num > buf_size) {
            num = 0;
            fflush(stdout);
            cur = printf("%s\n", "FLUSH!");
        }
        cur += printf("\n\nnum: %i\n", num);
        cur += printf("%i: Test Line\n", i + 1);
        cur += printf("cur: %i\n", cur);
        tot = num + cur;
        cur += printf("tot: %i\n", tot);
        num += cur;
        if (cur > maxcur) {
            maxcur = cur;
        }        
    }
    fflush(stdout);
    printf("\n\nmax: %i\n", maxcur);
    */

    ////
    //err = 0;
    //if ((err = fflush(stdout)) != 0 ) {
    //   fprintf(stderr, "error-2: %d\n", err);
    //   perror("fflush error\n");
    //}

    //fclose(stdout);
    //fseek(stdout, 0, SEEK_SET);
    //fflush(stdout);
    //printf("\nTOTAL FLUSH-777\n");

    lib_fs_files_free(files);                        
    free(dir_name);

    return 0;
}
