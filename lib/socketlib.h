#ifndef PLAZMA_LIB_SOCKETLIB_H
#define PLAZMA_LIB_SOCKETLIB_H

#include <stdint.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h> /* ioctl */
#include <signal.h>    /* sigaction, SIGPIPE */

//#include <sys/uio.h>

#endif

enum LIB_SOCKET_INIT {
    LIB_SOCKET_INIT_SIGPIPE  = 1, // UNIX: ignore SIGPIPE.  Use once per process.
    LIB_SOCKET_INIT_WSA      = 2, // Windows: initialize WSA library.  Use once per process.
    LIB_SOCKET_INIT_WSAFUNCS = 4, // Windows: get WSA functions.  Use once per module.
};

#define LIB_NF_TYPE_LOCAL 1
#define LIB_NF_TYPE_FTP   2
#define LIB_NF_TYPE_HTTP  3

#ifdef _WIN32

typedef SOCKET socket_fd_t;
//typedef WSABUF socket_iovec_t;

#define LIB_SOCKET_NULL  INVALID_SOCKET
#define LIB_SOCKET_NONBLOCK  0x0100
#define LIB_SOCKET_EINPROGRESS  ERROR_IO_PENDING
#else

typedef int socket_fd_t;
//typedef struct iovec socket_iovec_t;

#define LIB_SOCKET_NULL  (-1)
#define LIB_SOCKET_EINPROGRESS  EINPROGRESS

#if (defined __APPLE__ && defined __MACH__)
#define LIB_SOCKET_NONBLOCK  0x40000000
#else
#define LIB_SOCKET_NONBLOCK  SOCK_NONBLOCK
#endif

#endif

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

int lib_socket_init(int flags);

void lib_socket_close(socket_fd_t socket_fd);

int lib_socket_nonblock(socket_fd_t socket_fd, int nonblock);

socket_fd_t lib_socket_create(int domain, int type, int protocol);

ssize_t lib_socket_read(socket_fd_t fd, void* ptr, size_t len);

socket_fd_t lib_socket_connect(const char* host, int port);

ssize_t lib_socket_write(socket_fd_t fd, void* ptr, size_t len);

////

nf_file_t* net_ftp_parse_url(const char* fn, const char* mode);

int net_ftp_connect_file(nf_file_t* fp);

nf_file_t* net_http_parse_url(const char* fn, const char* mode);

int net_http_connect_file(nf_file_t* fp);

off_t net_read(nf_file_t* fp, void* buf, off_t len);

off_t net_seek(nf_file_t* fp, int64_t off, int whence);

nf_file_t* net_parse_url(const char* url, const char* mode);

int net_connect_file(nf_file_t* fp);

nf_file_t* net_open(const char* url, const char* mode);

char* net_get_file_contents(nf_file_t* fp, int* size);

#endif // PLAZMA_LIB_SOCKETLIB_H
