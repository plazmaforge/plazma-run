#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <cctype>

#include "socketlib.h"

static void socket_set_socketaddr(struct sockaddr_in *a, const char* host, int port) {
    //a->sin_len = sizeof(struct sockaddr_in); // WIN32 (?)
    a->sin_family = AF_INET;
    a->sin_port = htons(port);

    struct in_addr addr;
	addr.s_addr = inet_addr(host);

    a->sin_addr.s_addr = addr.s_addr;
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

static bool is_socket_init = false;

int socket_init_default() {
	if (is_socket_init) {
		return 0;
	}
	is_socket_init = true;
	return socket_init(LIB_SOCKET_INIT_WSA);
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
    return recv(fd, (char*) ptr, len, 0);
}

ssize_t socket_write(socket_fd_t fd, void* ptr, size_t len) {
    return send(fd, (char*) ptr, len, 0);
}
int socket_setopt(socket_fd_t fd, int level, int name, const void* val, int len) {
	return setsockopt(fd, level, name, (char*) val, len);
}

static int socket_getopt(socket_fd_t fd, int level, int name, void* val, int* len) {
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

int socket_init(int flags) {
    if (flags & LIB_SOCKET_INIT_SIGPIPE) {
        struct sigaction sa = {};
        sa.sa_handler = SIG_IGN;
        return sigaction(SIGPIPE, &sa, NULL);
    }
    return 0;
}

static bool is_socket_init = false;

int socket_init_default() {
	if (is_socket_init) {
		return 0;
	}
	is_socket_init = true;
	return 0; // Notning
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

int socket_setopt(socket_fd_t fd, int level, int name, const void* val, int len) {
	return setsockopt(fd, level, name, val, len);
}

static int socket_getopt(socket_fd_t fd, int level, int name, void* val, int* len) {
    return getsockopt(fd, level, name, val, (socklen_t*) len);
}
#endif

////

int socket_connect_fd(socket_fd_t socket_fd, const struct sockaddr* addr, int len) {
    if (0 != connect(socket_fd, /*(struct sockaddr*) &addr*/ addr, len))
        return -1;
    return 0;
}

socket_fd_t socket_connect(const char* host, int port) {

#ifdef _WIN32    
    socket_init_default();
#define __err_connect(func) do { fprintf(stderr, "%s: %d\n", func, WSAGetLastError());	return LIB_SOCKET_NULL;} while (0)
#else
#define __err_connect(func) do { perror(func); /*freeaddrinfo(res);*/ return LIB_SOCKET_NULL; } while (0)
#endif

	socket_fd_t fd;
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

	if ((fd = socket_create(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == LIB_SOCKET_NULL) __err_connect("socket");

	/* The following two setsockopt() are used by ftplib
	 * (http://nbpfaus.net/~pfau/ftplib/). I am not sure if they
	 * necessary. */

// #ifdef _WIN32  
// 	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(on)) == -1) __err_connect("setsockopt");
// 	if (setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*) &lng, sizeof(lng)) == -1) __err_connect("setsockopt");
// #else
// 	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) __err_connect("setsockopt");
// 	if (setsockopt(fd, SOL_SOCKET, SO_LINGER, &lng, sizeof(lng)) == -1) __err_connect("setsockopt");
// #endif

	if (socket_setopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) __err_connect("setsockopt");
	if (socket_setopt(fd, SOL_SOCKET, SO_LINGER, &lng, sizeof(lng)) == -1) __err_connect("setsockopt");

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

	if (socket_connect_fd(fd, (struct sockaddr*) &server, sizeof(server)) != 0) __err_connect("connect");

	//freeaddrinfo(res);

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

const char* HTTP_PREFIX = "http://";
const char* HTTPS_PREFIX = "https://";
const int HTTP_PORT = 80;

const char* http_get_url_prefix(const char* url) {
	if (!url) {
		return NULL;
	}
	if (strstr(url, HTTP_PREFIX) == url) {
		return HTTP_PREFIX;
	}
	if (strstr(url, HTTPS_PREFIX) == url) {
		return HTTPS_PREFIX;
	}
	return NULL;
}

char* lib_strchr_end(char* str, int c) {
	if (!str) {
		return NULL;
	}
	char* val;
	for (val = str; *val && *val != c; ++val);
	return val;
}

nf_file_t* http_parse_url(const char* url, const char* mode) {
	if (!url) {
		return NULL;
	}
	const char* prefix = http_get_url_prefix(url);
	printf("prefix %s\n", prefix);

	if (!prefix) {
		return NULL;
	}
	int prefix_len = strlen(prefix);
	if (url[prefix_len] == '\0') {
		return NULL;
	}

	nf_file_t* fp;
	char* proxy;
	char *p;
	char* q;
	int len;

	// set ->http_host
	p = lib_strchr_end(((char*) url) + prefix_len, '/');
	len = p - url - prefix_len;

	fp = (nf_file_t*) calloc(1, sizeof(nf_file_t));
	fp->http_host = (char*) calloc(len + 1, 1);
	strncpy(fp->http_host, url + prefix_len, len);
	fp->http_host[len] = 0;

	q = lib_strchr_end(fp->http_host, ':');
	if (*q == ':') *q++ = 0;

	// get http_proxy
	proxy = getenv("http_proxy");

	// set ->host, ->port and ->path
	if (!proxy) {
		fp->host = strdup(fp->http_host); // when there is no proxy, server name is identical to http_host name.
		fp->port = *q ? atoi(q) : HTTP_PORT;
		fp->path = strdup(*p ? p : "/");
	} else {
		prefix = http_get_url_prefix(proxy);
		fp->host = prefix ? strdup(proxy + strlen(prefix)) : strdup(proxy);
		q = lib_strchr_end(fp->host, ':');
		if (*q == ':') *q++ = 0; 
		fp->port = *q? atoi(q) : HTTP_PORT;
		fp->path = strdup(url);
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
	int len = 0;
	char* buf;
	char* p;

	buf = (char*) calloc(0x10000, 1);
	
    len += sprintf(buf + len, "GET %s HTTP/1.0\r\nHost: %s\r\n", fp->path, fp->host);
    len += sprintf(buf + len, "Range: bytes=%lld-\r\n", (long long) fp->offset);
	len += sprintf(buf + len, "\r\n");

	int ssize = socket_write(fp->fd, buf, len);

	printf("\n");
	printf("write_size: %i\n", ssize);
	printf("\n");
	printf("%s\n", buf);

	len = 0;
	while (socket_read(fp->fd, buf + len, 1)) { // read HTTP header
		//printf("%c", buf[len]);
		if (buf[len] == '\n' && len >= 3)
			if (strncmp(buf + len - 3, "\r\n\r\n", 4) == 0) break;
		++len;
	}

	printf("%s\n", buf);

	buf[len] = 0;
	if (len < 14) { // prematured header
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

