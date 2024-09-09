
#ifndef PLAZMA_LIB_NETLIB_H
#define PLAZMA_LIB_NETLIB_H

#define LIB_NF_TYPE_LOCAL 1
#define LIB_NF_TYPE_FTP   2
#define LIB_NF_TYPE_HTTP  3

#include "socketlib.h"

typedef struct nf_file_s {
	int type;
    socket_fd_t fd;
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

} nf_file_t;


nf_file_t* lib_net_ftp_parse_url(const char* fn, const char* mode);

int lib_net_ftp_connect_file(nf_file_t* fp);

nf_file_t* net_http_parse_url(const char* fn, const char* mode);

int lib_net_http_connect_file(nf_file_t* fp);

off_t lib_net_read(nf_file_t* fp, void* buf, off_t len);

off_t lib_net_seek(nf_file_t* fp, int64_t off, int whence);

nf_file_t* lib_net_parse_url(const char* url, const char* mode);

int lib_net_connect_file(nf_file_t* fp);

nf_file_t* lib_net_open(const char* url, const char* mode);

char* lib_net_get_file_contents(nf_file_t* fp, int* size);

#endif // PLAZMA_LIB_NETLIB_H

