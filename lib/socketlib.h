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

int lib_socket_init(int flags);

void lib_socket_close(socket_fd_t socket_fd);

int lib_socket_nonblock(socket_fd_t socket_fd, int nonblock);

socket_fd_t lib_socket_create(int domain, int type, int protocol);

ssize_t lib_socket_read(socket_fd_t fd, void* ptr, size_t len);

socket_fd_t lib_socket_connect(const char* host, int port);

ssize_t lib_socket_write(socket_fd_t fd, void* ptr, size_t len);

////

int lib_socket_wait(int fd, int is_read);

////

void print_buf_v1(const char* buf, int len, const char* marker);

void print_buf_v2(const char* buf, int len, const char* marker);

void print_buf(const char* buf, int len, const char* marker);

void print_test_buf(const char* buf, int len);

#endif // PLAZMA_LIB_SOCKETLIB_H
