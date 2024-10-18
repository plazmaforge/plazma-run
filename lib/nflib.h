
#ifndef PLAZMA_LIB_NETLIB_H
#define PLAZMA_LIB_NETLIB_H

#define LIB_NF_TYPE_LOCAL 1
#define LIB_NF_TYPE_FTP   2
#define LIB_NF_TYPE_HTTP  3

#include "socketlib.h"

typedef struct lib_nf_file_s {
	int type;
    lib_socket_fd_t fd;
	int64_t offset;
	char *host;
    int port;

    // HTTP
    char* path;
    char* http_host;

    // FTP
    int is_ready;
    int64_t seek_offset; // for lazy seek
    int64_t file_size;

} lib_nf_file_t;


lib_nf_file_t* lib_nf_ftp_parse_url(const char* fn, const char* mode);

int lib_nf_ftp_connect_file(lib_nf_file_t* fp);

lib_nf_file_t* lib_nf_http_parse_url(const char* fn, const char* mode);

int lib_nf_http_connect_file(lib_nf_file_t* fp);

off_t lib_nf_read(lib_nf_file_t* fp, void* buf, off_t len);

off_t lib_nf_seek(lib_nf_file_t* fp, int64_t off, int whence);

lib_nf_file_t* lib_nf_parse_url(const char* url, const char* mode);

int lib_nf_connect_file(lib_nf_file_t* fp);

lib_nf_file_t* lib_nf_open(const char* url, const char* mode);

char* lib_nf_get_file_contents(lib_nf_file_t* fp, int* size);

#endif // PLAZMA_LIB_NETLIB_H

