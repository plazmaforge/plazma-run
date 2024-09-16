#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <cctype>

#include "socketlib.h"

static void lib_socket_set_socketaddr(struct sockaddr_in *a, const char* host, int port) {
    //a->sin_len = sizeof(struct sockaddr_in); // WIN32 (?)
    a->sin_family = AF_INET;
    a->sin_port = htons(port);

    struct in_addr addr;
	addr.s_addr = inet_addr(host);

    a->sin_addr.s_addr = addr.s_addr;
}

static lib_socket_fd_t lib_socket_create_nonblock(int domain, int type, int protocol) {
    lib_socket_fd_t socket_fd = socket(domain, type & ~LIB_SOCKET_NONBLOCK, protocol);
    if ((type & LIB_SOCKET_NONBLOCK) && socket_fd != LIB_SOCKET_NULL 
        && 0 != lib_socket_nonblock(socket_fd, 1)) {
        lib_socket_close(socket_fd);
        return LIB_SOCKET_NULL;
    }
    return socket_fd;
}

#ifdef _WIN32

static bool is_socket_init = false;

int lib_socket_init(int flags) {
    if (flags & LIB_SOCKET_INIT_WSA) {
        WSADATA wsa;
        if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
            return -1;
    }

    //if (flags & LIB_SOCKET_INIT_WSAFUNCS) {
    //    if (0 != _socket_wsa_get_funcs())
    //        return -1;
    //}

    return 0;
}

int lib_socket_init_default() {
	if (is_socket_init) {
		return 0;
	}
	is_socket_init = true;
	return lib_socket_init(LIB_SOCKET_INIT_WSA);
}

void lib_socket_close(lib_socket_fd_t socket_fd) {
    if (socket_fd == LIB_SOCKET_NULL) return;
    closesocket(socket_fd);
}

int lib_socket_nonblock(lib_socket_fd_t socket_fd, int nonblock) {
    return ioctlsocket(socket_fd, FIONBIO, (unsigned long*) &nonblock);
}

lib_socket_fd_t lib_socket_create(int domain, int type, int protocol) {
    return lib_socket_create_nonblock(domain, type, protocol);
}

ssize_t lib_socket_read(lib_socket_fd_t fd, void* ptr, size_t len) {
    return recv(fd, (char*) ptr, len, 0);
}

ssize_t lib_socket_write(lib_socket_fd_t fd, void* ptr, size_t len) {
    return send(fd, (char*) ptr, len, 0);
}

int lib_socket_setopt(lib_socket_fd_t fd, int level, int name, const void* val, int len) {
	return setsockopt(fd, level, name, (char*) val, len);
}

static int lib_socket_getopt(lib_socket_fd_t fd, int level, int name, void* val, int* len) {
    return getsockopt(fd, level, name, (char*) val, len);
}
#else

// #include <pthread.h>
// #include <sys/socket.h>
// #include <sys/un.h>
// #include <sys/types.h>
// #include <netinet/in.h>
// #include <netinet/tcp.h>

//#include <fcntl.h>
//#include <arpa/inet.h>
//#include <sys/time.h>
//#include <netdb.h>
//#include <errno.h>

int lib_socket_init(int flags) {
    if (flags & LIB_SOCKET_INIT_SIGPIPE) {
        struct sigaction sa = {};
        sa.sa_handler = SIG_IGN;
        return sigaction(SIGPIPE, &sa, NULL);
    }
    return 0;
}

void lib_socket_close(lib_socket_fd_t socket_fd) {
    if (socket_fd == LIB_SOCKET_NULL) return;
    close(socket_fd);
}

int lib_socket_nonblock(lib_socket_fd_t socket_fd, int nonblock) {
    return ioctl(socket_fd, FIONBIO, &nonblock);
}

lib_socket_fd_t lib_socket_create(int domain, int type, int protocol) {
#if (defined __APPLE__ && defined __MACH__)
    lib_socket_fd_t socket_fd = lib_socket_create_nonblock(domain, type, protocol);
    return socket_fd;
#else
    return socket(domain, type, protocol);
#endif
}

ssize_t lib_socket_read(lib_socket_fd_t fd, void* ptr, size_t len) {
    return read(fd, ptr, len);
	//return recv(fd, ptr, len, 0);
}

ssize_t lib_socket_write(lib_socket_fd_t fd, void* ptr, size_t len) {
    return write(fd, ptr, len);
}

int lib_socket_setopt(lib_socket_fd_t fd, int level, int name, const void* val, int len) {
	return setsockopt(fd, level, name, val, len);
}

static int lib_socket_getopt(lib_socket_fd_t fd, int level, int name, void* val, int* len) {
    return getsockopt(fd, level, name, val, (socklen_t*) len);
}
#endif

////

int lib_socket_connect_fd(lib_socket_fd_t socket_fd, const struct sockaddr* addr, int len) {
    if (0 != connect(socket_fd, /*(struct sockaddr*) &addr*/ addr, len))
        return -1;
    return 0;
}

lib_socket_fd_t lib_socket_connect(const char* host, int port) {

#ifdef _WIN32    
    lib_socket_init_default();
#define __err_connect(func) do { fprintf(stderr, "%s: %d\n", func, WSAGetLastError());	return LIB_SOCKET_NULL;} while (0)
#else
#define __err_connect(func) do { perror(func); /*freeaddrinfo(res);*/ return LIB_SOCKET_NULL; } while (0)
#endif

	lib_socket_fd_t fd;
	int on = 1;
	struct linger lng = { 0, 0 };

	//int ai_err
	//struct addrinfo hints, *res = 0;
	//memset(&hints, 0, sizeof(struct addrinfo));
	//hints.ai_family = AF_UNSPEC;
	//hints.ai_socktype = SOCK_STREAM;

	/* In Unix/Mac, getaddrinfo() is the most convenient way to get
	 * server information. */
	//char port_s[4];
	//sprintf(port_s, "%i", port);

	//if ((ai_err = getaddrinfo(host, port_s, &hints, &res)) != 0) { fprintf(stderr, "can't resolve %s:%s: %s\n", host, port_s, gai_strerror(ai_err)); return -1; }
	//if ((fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) __err_connect("socket");

	if ((fd = lib_socket_create(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == LIB_SOCKET_NULL) __err_connect("socket");

	/* The following two setsockopt() are used by ftplib
	 * (http://nbpfaus.net/~pfau/ftplib/). I am not sure if they
	 * necessary. */

	if (lib_socket_setopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) __err_connect("setsockopt");
	if (lib_socket_setopt(fd, SOL_SOCKET, SO_LINGER, &lng, sizeof(lng)) == -1) __err_connect("setsockopt");

	//if (connect(fd, res->ai_addr, res->ai_addrlen) != 0) __err_connect("connect");
	//if (socket_connect_fd(fd, res->ai_addr, res->ai_addrlen) != 0) __err_connect("connect");

    // struct sockaddr_in server;
    // socket_set_socketaddr(&server, host, port);
    // const struct sockaddr* addr = (struct sockaddr*) &server;

	struct sockaddr_in server;
	struct hostent *hp = 0;
		if (isalpha(host[0])) hp = gethostbyname(host);
	else {
		struct in_addr addr;
		addr.s_addr = inet_addr(host);
		hp = gethostbyaddr((char*) &addr, 4, AF_INET);
	}
	if (hp == 0) __err_connect("gethost");

	// connect
	server.sin_addr.s_addr = *((unsigned long*) hp->h_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if (lib_socket_connect_fd(fd, (struct sockaddr*) &server, sizeof(server)) != 0) __err_connect("connect");

	//freeaddrinfo(res);

	return fd;
}

////

int lib_socket_wait(int fd, int is_read) {
	fd_set fds, *fdr = 0, *fdw = 0;
	struct timeval tv;
	int ret;
	tv.tv_sec = 5;  // 1 seconds time out
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	if (is_read) fdr = &fds;
	else fdw = &fds;
	ret = select(fd + 1, fdr, fdw, 0, &tv);
#ifndef _WIN32
	if (ret == -1) perror("select");
#else
	if (ret == 0)
		fprintf(stderr, "select time-out\n");
	else if (ret == SOCKET_ERROR)
		fprintf(stderr, "select: %d\n", WSAGetLastError());
#endif
    //printf("socket_wait: end\n");
	return ret;
}



////

void lib_socket_print_buf_v1(const char* buf, int len, const char* marker) {
	if (!buf || len <= 0) {
		return;
	}
	char c = 0;
	int i = 0;
	bool line = true;
	while (i < len) {
		if (line) {
			printf("%s", marker);
		}
		line = false;
		c = buf[i];
		if (c == '\r') {
			if (i + 1 < len && buf[i + 1] == '\n') {
				// shift '\n'
				i++;
			}
			line = true;
		} else if (c == '\n') {
			line = true;
		}
		if (line) {
			printf("\n");
		} else {
			printf("%c", c);
		}
		i++;
	}
}

void lib_socket_print_buf_v2(const char* buf, int len, const char* marker) {
	if (!buf || len <= 0) {
		return;
	}
	char c = 0;
	int i = 0;
	bool line = true;
	while (i < len) {
		if (line) {
			if (i > 0) {
			    printf("\n");
		    }
			printf("%s", marker);
		}
		line = false;
		c = buf[i];
		if (c == '\r') {
			if (i + 1 == len) {
				// last '\r'
				printf("\n");
				break;
			} else {
				if (i + 2 == len - 1 && strncmp(buf + i, "\r\n\r", 3) == 0) {
					// last '\r\n\r'
					printf("\n");
					break;
				} else if (i + 3 == len - 1 && strncmp(buf + i, "\r\n\r\n", 4) == 0) {
					// last '\r\n\r\n'
					printf("\n");
					break;
				} else if (buf[i + 1] == '\n') {
					// shift '\n'
					i++;
				}
			}
			line = true;
		} else if (c == '\n') {
			if (i + 1 == len) {
				// last '\n'
				printf("\n");
				break;
			}
			line = true;
		}
		if (!line) {
			printf("%c", c);
		}	
		i++;
	}
}

void lib_socket_print_buf(const char* buf, int len, const char* marker) {
	lib_socket_print_buf_v1(buf, len, marker);
}

void lib_socket_print_test_buf(const char* buf, int len) {
	if (!buf || len <= 0) {
		return;
	}
	char c = 0;
	int i = 0;
	bool line = true;
	while (i < len) {
		c = buf[i];
		if (c == '\r') {
			printf("[R]");
		} else if (c == '\n') {
			printf("[N]");
		} else {
			printf("%c", c);
		}	
		i++;
	}
}

////
