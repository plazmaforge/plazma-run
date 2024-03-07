#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "socketlib.h"

static void socket_set_socketaddr(struct sockaddr_in *a, const char* host, int port) {
    a->sin_len = sizeof(struct sockaddr_in);
    a->sin_family = AF_INET;
    a->sin_port = port;

    struct in_addr addr;
	addr.s_addr = inet_addr(host);

    a->sin_addr = addr;
}

static socket_fd_t socket_create_nonblock(int domain, int type, int protocol) {
    socket_fd_t socket_fd = socket(domain, type & ~LIB_SOCKET_NONBLOCK, protocol);
    if ((type & LIB_SOCKET_NONBLOCK) && socket_fd != LIB_SOCKET_NULL 
        && 0 != socket_nonblock(socket_fd, 1)) {
        socket_close(socket_fd);
        return LIB_SOCKET_NULL;
    }
    return socket_fd;
}

#ifdef _WIN32

int socket_init(int flags) {
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

void socket_close(socket_fd_t socket_fd) {
    if (socket_fd == LIB_SOCKET_NULL) return;
    closesocket(socket_fd);
}

int socket_nonblock(socket_fd_t socket_fd, int nonblock) {
    return ioctlsocket(socket_fd, FIONBIO, (unsigned long*) &nonblock);
}

socket_fd_t socket_create(int domain, int type, int protocol) {
    return socket_create_nonblock(domain, type, protocol);
}

ssize_t socket_read(socket_fd_t fd, void* ptr, size_t len) {
    return recv(fd, ptr, len, 0);
}

ssize_t socket_write(socket_fd_t fd, void* ptr, size_t len) {
    return send(fd, ptr, len, 0);
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

int socket_init(int flags) {
    if (flags & LIB_SOCKET_INIT_SIGPIPE) {
        struct sigaction sa = {};
        sa.sa_handler = SIG_IGN;
        return sigaction(SIGPIPE, &sa, NULL);
    }
    return 0;
}

void socket_close(socket_fd_t socket_fd) {
    if (socket_fd == LIB_SOCKET_NULL) return;
    close(socket_fd);
}

int socket_nonblock(socket_fd_t socket_fd, int nonblock) {
    return ioctl(socket_fd, FIONBIO, &nonblock);
}

socket_fd_t socket_create(int domain, int type, int protocol) {
#if (defined __APPLE__ && defined __MACH__)
    socket_fd_t socket_fd = socket_create_nonblock(domain, type, protocol);
    return socket_fd;
#else
    return socket(domain, type, protocol);
#endif
}

ssize_t socket_read(socket_fd_t fd, void* ptr, size_t len) {
    return read(fd, ptr, len);
}

ssize_t socket_write(socket_fd_t fd, void* ptr, size_t len) {
    return write(fd, ptr, len);
}
#endif

////

int socket_connect_fd(socket_fd_t socket_fd, const sockaddr* addr, int len) {
    if (0 != connect(socket_fd, (struct sockaddr*) &addr, len))
        return -1;
    return 0;
}

socket_fd_t socket_connect(const char* host, int port) {
    
	// socket_fd_t fd;
    // if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == LIB_SOCKET_NULL) {
    //     return LIB_SOCKET_NULL;
    // }

    // struct sockaddr_in server;
    // socket_set_socketaddr(&server, host, port);
    // const struct sockaddr* addr = (struct sockaddr*) &server;

    // if (socket_connect_fd(fd, addr, addr->sa_len) != 0) {
	// 	printf("POINT-ERR-2\n");
    //     return LIB_SOCKET_NULL;
    // }

#define __err_connect(func) do { perror(func); freeaddrinfo(res); return -1; } while (0)

	int ai_err, on = 1, fd;
	struct linger lng = { 0, 0 };
	struct addrinfo hints, *res = 0;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	/* In Unix/Mac, getaddrinfo() is the most convenient way to get
	 * server information. */

	if ((ai_err = getaddrinfo(host, "80", &hints, &res)) != 0) { fprintf(stderr, "can't resolve %s:%s: %s\n", host, "80", gai_strerror(ai_err)); return -1; }
	if ((fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) __err_connect("socket");

	/* The following two setsockopt() are used by ftplib
	 * (http://nbpfaus.net/~pfau/ftplib/). I am not sure if they
	 * necessary. */

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) __err_connect("setsockopt");
	if (setsockopt(fd, SOL_SOCKET, SO_LINGER, &lng, sizeof(lng)) == -1) __err_connect("setsockopt");

	if (connect(fd, res->ai_addr, res->ai_addrlen) != 0) __err_connect("connect");
	freeaddrinfo(res);

	return fd;
}

////

static int socket_wait(int fd, int is_read) {
	fd_set fds, *fdr = 0, *fdw = 0;
	struct timeval tv;
	int ret;
	tv.tv_sec = 5; tv.tv_usec = 0; // 5 seconds time out
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
	return ret;
}


off_t _net_read(socket_fd_t fd, void *buf, off_t len) {

	off_t rest = len, curr, l = 0;

	/* recv() and read() may not read the required length of data with
	 * one call. They have to be called repeatedly. */

	while (rest) {
		if (socket_wait(fd, 1) <= 0) break; // socket is not ready for reading
		curr = socket_read(fd, (char*) buf + l, rest); // TODO (!) void* -> char*

		/* According to the glibc manual, section 13.2, a zero returned
		 * value indicates end-of-file (EOF), which should mean that
		 * read() will not return zero if EOF has not been met but data
		 * are not immediately available. */

		if (curr == 0) break;
		l += curr; rest -= curr;
	}
	return l;
}

off_t net_read(nf_file_t* fp, void* buf, off_t len) {

	off_t l = 0;
	if (fp->fd == -1) {
		return 0;
	}

	// if (fp->type == LIB_NF_TYPE_FTP) {
	// 	if (fp->is_ready == 0) {
	// 		if (!fp->no_reconnect) ftp_reconnect(fp);
	// 		ftp_connect_file(fp);
	// 	}

	// } 
	
	if (fp->type == LIB_NF_TYPE_HTTP) {
		if (fp->is_ready == 0) {
			http_connect_file(fp);
		}
			
	}

	if (fp->type == LIB_NF_TYPE_LOCAL) { // on Windows, the following block is necessary; not on UNIX
		off_t rest = len, curr;
		while (rest) {
			do {
				curr = read(fp->fd, (char*) buf + l, rest); // TODO (!) void* -> char*
			} while (curr < 0 && EINTR == errno);
			if (curr < 0) return -1;
			if (curr == 0) break;
			l += curr; rest -= curr;
		}
	} else {
		l = _net_read(fp->fd, buf, len);
	}
	fp->offset += l;

	return l;
}


off_t net_seek(nf_file_t* fp, int64_t off, int whence) {

	if (whence == SEEK_SET && off == fp->offset) return 0;

	if (fp->type == LIB_NF_TYPE_LOCAL) {
		/* Be aware that lseek() returns the offset after seeking,
		 * while fseek() returns zero on success. */
		off_t offset = lseek(fp->fd, off, whence);
		if (offset == -1) {
            // Be silent, it is OK for net_seek to fail when the file is streamed
            // fprintf(stderr,"[net_seek] %s\n", strerror(errno));
			return -1;
		}
		fp->offset = offset;
		return off;

	} 
	
	if (fp->type == LIB_NF_TYPE_FTP) {

        if (whence == SEEK_CUR)
            fp->offset += off;
        else if (whence == SEEK_SET)
            fp->offset = off;
        else if ( whence == SEEK_END)
            fp->offset = fp->file_size + off;

		fp->is_ready = 0;
		return off;

	}
	
	if (fp->type == LIB_NF_TYPE_HTTP) {
		if (whence == SEEK_END) { // FIXME: can we allow SEEK_END in future?
			fprintf(stderr, "[net_seek] SEEK_END is not supported for HTTP. Offset is unchanged.\n");
			errno = ESPIPE;
			return -1;
		}
        if (whence==SEEK_CUR)
            fp->offset += off;
        else if (whence==SEEK_SET)
            fp->offset = off;
		fp->is_ready = 0;
		return off;
	}

	errno = EINVAL;
    fprintf(stderr,"[net_seek] %s\n", strerror(errno));
	return -1;
}

////

nf_file_t* http_parse_url(const char* fn, const char* mode) {
	nf_file_t* fp;
	char *p, *proxy, *q;
	int l;

	if (strstr(fn, "http://") != fn) return 0;

	int proto_len = 7; // 8

	// set ->http_host
	for (p = (char*) fn + proto_len; *p && *p != '/'; ++p);
	l = p - fn - proto_len;

	fp = (nf_file_t*) calloc(1, sizeof(nf_file_t));
	fp->http_host = (char*) calloc(l + 1, 1);
	strncpy(fp->http_host, fn + proto_len, l);
	fp->http_host[l] = 0;

	for (q = fp->http_host; *q && *q != ':'; ++q);
	if (*q == ':') *q++ = 0;

	// get http_proxy
	proxy = getenv("http_proxy");

	// set ->host, ->port and ->path
	if (proxy == 0) {
		fp->host = strdup(fp->http_host); // when there is no proxy, server name is identical to http_host name.
		//fp->port = strdup(*q? q : "80");
		fp->port = 80; // TODO
		fp->path = strdup(*p ? p : "/");
	} else {
		fp->host = (strstr(proxy, "http://") == proxy)? strdup(proxy + proto_len) : strdup(proxy);
		for (q = fp->host; *q && *q != ':'; ++q);
		if (*q == ':') *q++ = 0; 
		//fp->port = strdup(*q ? q : "80");
		fp->port = 80; // TODO
		fp->path = strdup(fn);
	}

	fp->type = LIB_NF_TYPE_HTTP;
	//fp->ctrl_fd = fp->fd = -1;

	fp->fd = -1;
	fp->seek_offset = 0;
	return fp;
}

////

int http_connect_file(nf_file_t* fp /*const char* host, int port, const char* path*/) {

	fp->fd = socket_connect(fp->host, fp->port);

	if (fp->fd == LIB_SOCKET_NULL) {
		fprintf(stderr, "Cannot connect: host: %s port: %i", fp->host, fp->port);
		fp->fd = -1;
		return -1;
	}

	int ret;
	int l = 0;
	char* buf;
	char* p;

	buf = (char*) calloc(0x10000, 1);
	
    l += sprintf(buf + l, "GET %s HTTP/1.0\r\nHost: %s\r\n", fp->path, fp->host);
    l += sprintf(buf + l, "Range: bytes=%lld-\r\n", (long long) fp->offset);
	l += sprintf(buf + l, "\r\n");

	int ssize = socket_write(fp->fd, buf, l);

	printf("\n");
	printf("write_size: %i\n", ssize);
	printf("\n");
	printf("%s\n", buf);

	l = 0;
	while (socket_read(fp->fd, buf + l, 1)) { // read HTTP header
		printf("%c", buf[l]); // TODO
		if (buf[l] == '\n' && l >= 3)
			if (strncmp(buf + l - 3, "\r\n\r\n", 4) == 0) break;
		++l;
	}

	buf[l] = 0;
	if (l < 14) { // prematured header
	    //printf("Header\n");
		socket_close(fp->fd);
		fp->fd = -1;
		return -1;
	}

	ret = strtol(buf + 8, &p, 0); // HTTP return code
	if (ret == 200 && fp->offset > 0) { // 200 (complete result); then skip beginning of the file
	    //printf("Body\n");
		off_t rest = fp->offset;
		while (rest) {
			off_t l = rest < 0x10000? rest : 0x10000;
			rest -= _net_read((int) fp->fd, buf, l);
		}
	} else if (ret != 206 && ret != 200) {
		free(buf);
		fprintf(stderr, "[http_connect_file] fail to open file (HTTP code: %d).\n", ret);
		socket_close(fp->fd);
		fp->fd = -1;
		return -1;
	}

	free(buf);
	fp->is_ready = 1;
	return 0;
}

