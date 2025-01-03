#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "strlib.h"
#include "nflib.h"

////

const char* LIB_FTP_PREFIX = "ftp://";
const char* LIB_FTPS_PREFIX = "ftps://";
const int LIB_FTP_PORT = 21;

const char* LIB_HTTP_PREFIX = "http://";
const char* LIB_HTTPS_PREFIX = "https://";
const int LIB_HTTP_PORT = 80;

////

off_t _lib_nf_read(lib_socket_fd_t fd, void *buf, off_t len) {

	off_t rest = len, curr, l = 0;

	/* recv() and read() may not read the required length of data with
	 * one call. They have to be called repeatedly. */

	while (rest) {
		if (lib_socket_wait(fd, 1) <= 0) break; // socket is not ready for reading
		curr = lib_socket_read(fd, (char*) buf + l, rest); // TODO (!) void* -> char*

		/* According to the glibc manual, section 13.2, a zero returned
		 * value indicates end-of-file (EOF), which should mean that
		 * read() will not return zero if EOF has not been met but data
		 * are not immediately available. */

		if (curr == 0) break;
		l += curr; 
		rest -= curr;
	}
	return l;
}

off_t lib_nf_read(lib_nf_file_t* fp, void* buf, off_t len) {

	off_t l = 0;
	if (fp->fd == -1) {
		return 0;
	}

	// if (fp->type == LIB_NF_TYPE_FTP) {
	// 	if (fp->is_ready == 0) {
	// 		if (!fp->no_reconnect) lin_nf_ftp_reconnect(fp);
	// 		lib_nf_ftp_connect_file(fp);
	// 	}
	// } 
	
	if (fp->type == LIB_NF_TYPE_HTTP) {
		if (fp->is_ready == 0) {
			lib_nf_http_connect_file(fp);
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
		l = _lib_nf_read(fp->fd, buf, len);
	}
	fp->offset += l;

	return l;
}


off_t lib_nf_seek(lib_nf_file_t* fp, int64_t off, int whence) {

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
        if (whence == SEEK_CUR)
            fp->offset += off;
        else if (whence == SEEK_SET)
            fp->offset = off;
		fp->is_ready = 0;
		return off;
	}

	errno = EINVAL;
    fprintf(stderr,"[net_seek] %s\n", strerror(errno));
	return -1;
}

////

bool lib_nf_is_ftp_url(const char* url) {
	if (!url) {
		return false;
	}
	return (strstr(url, LIB_FTP_PREFIX) == url || strstr(url, LIB_FTPS_PREFIX) == url);
}

bool lib_nf_is_http_url(const char* url) {
	if (!url) {
		return false;
	}
	return (strstr(url, LIB_HTTP_PREFIX) == url || strstr(url, LIB_HTTPS_PREFIX) == url);
}

const char* lib_nf_get_ftp_prefix(const char* url) {
	if (!url) {
		return NULL;
	}

	// FTP
	if (strstr(url, LIB_FTP_PREFIX) == url) {
		return LIB_FTP_PREFIX;
	}
	if (strstr(url, LIB_FTPS_PREFIX) == url) {
		return LIB_FTPS_PREFIX;
	}

	return NULL;
}

const char* lib_nf_get_http_prefix(const char* url) {
	if (!url) {
		return NULL;
	}

	// HTTP
	if (strstr(url, LIB_HTTP_PREFIX) == url) {
		return LIB_HTTP_PREFIX;
	}
	if (strstr(url, LIB_HTTPS_PREFIX) == url) {
		return LIB_HTTPS_PREFIX;
	}

	return NULL;
}

const char* lib_nf_get_url_prefix2(const char* url) {
	if (!url) {
		return NULL;
	}
	const char* prefix = NULL;

	// FTP
	prefix = lib_nf_get_ftp_prefix(url);
	if (prefix) {
		return prefix;
	}

	// HTTP
	prefix = lib_nf_get_http_prefix(url);
	if (prefix) {
		return prefix;
	}

	return NULL;
}

////

lib_nf_file_t* lib_nf_ftp_parse_url(const char* url, const char* mode) {
	if (!url) {
		fprintf(stderr, "Cannot parse FTP url. Url is NULL\n");
		return NULL;
	}
	const char* prefix = lib_nf_get_ftp_prefix(url);

	if (!prefix) {
		fprintf(stderr, "Cannot parse FTP url: %s. Incorrect protocol\n", url);
		return NULL;
	}
	int prefix_len = strlen(prefix);
	if (url[prefix_len] == '\0') {
		fprintf(stderr, "Cannot parse FTP url: %s. Path is empty\n", url);
		return NULL;
	}

	lib_nf_file_t* fp;

	fp = (lib_nf_file_t*) calloc(1, sizeof(lib_nf_file_t));

	// TODO

	fp->type = LIB_NF_TYPE_FTP;
	//fp->ctrl_fd = fp->fd = -1;

	fp->fd = -1;
	fp->seek_offset = 0;
	return fp;
}

int lib_nf_ftp_connect_file(lib_nf_file_t* fp) {
	// TODO
	return -1;
}

lib_nf_file_t* lib_nf_http_parse_url(const char* url, const char* mode) {
	if (!url) {
		fprintf(stderr, "Cannot parse HTTP url. Url is NULL\n");
		return NULL;
	}
	const char* prefix = lib_nf_get_http_prefix(url);

	if (!prefix) {
		fprintf(stderr, "Cannot parse HTTP url: %s. Incorrect protocol\n", url);
		return NULL;
	}

	int prefix_len = strlen(prefix);
	if (url[prefix_len] == '\0') {
		fprintf(stderr, "Cannot parse HTTP url: %s. Path is empty\n", url);
		return NULL;
	}

	lib_nf_file_t* fp;
	char* proxy;
	char *p;
	char* q;
	int len;

	// set ->http_host
	p = lib_strchr_end(((char*) url) + prefix_len, '/');
	len = p - url - prefix_len;

	fp = (lib_nf_file_t*) calloc(1, sizeof(lib_nf_file_t));
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
		fp->port = *q ? atoi(q) : LIB_HTTP_PORT;
		fp->path = strdup(*p ? p : "/");
	} else {
		prefix = lib_nf_get_http_prefix(proxy);
		fp->host = prefix ? strdup(proxy + strlen(prefix)) : strdup(proxy);
		q = lib_strchr_end(fp->host, ':');
		if (*q == ':') *q++ = 0; 
		fp->port = *q? atoi(q) : LIB_HTTP_PORT;
		fp->path = strdup(url);
	}

	fp->type = LIB_NF_TYPE_HTTP;
	//fp->ctrl_fd = fp->fd = -1;

	fp->fd = -1;
	fp->seek_offset = 0;
	return fp;
}

////

int lib_nf_http_connect_file(lib_nf_file_t* fp) {
	if (!fp) {
		return -1;
	}

	fp->fd = lib_socket_connect(fp->host, fp->port);

	if (fp->fd == LIB_SOCKET_NULL) {
		fprintf(stderr, "Cannot open HTTP connection. host: %s port: %i\n", fp->host, fp->port);
		fp->fd = -1;
		return -1;
	}

	int ret;
	int len = 0;
	char* buf;
	char* p;

	bool verbose = true;
	bool separate = false;
	buf = (char*) calloc(65536, 1);

	/*
	 WARNING! HTTP/1.1 is slow
	*/

    len += sprintf(buf + len, "GET %s HTTP/1.0\r\nHost: %s\r\n", fp->path, fp->host);
    len += sprintf(buf + len, "Range: bytes=%lld-\r\n", (long long) fp->offset);
	len += sprintf(buf + len, "\r\n");

	int ssize = lib_socket_write(fp->fd, buf, len);

	//printf("\n");
	//printf("write_size: %i\n", ssize);
	//printf("\n");

	if (verbose && ssize > 0) {
		if (separate) {
			printf("\n");
		}
		lib_socket_print_buf(buf, ssize, "> ");
		//lib_socket_print_test_buf(buf, ssize);
	}

	len = 0;
	while (lib_socket_read(fp->fd, buf + len, 1)) { // read HTTP header
		//printf("%c", buf[len]);
		if (buf[len] == '\n' && len >= 3)
			if (strncmp(buf + len - 3, "\r\n\r\n", 4) == 0) break;
		++len;
	}

	if (verbose && len > 0) {
		if (separate) {
			printf("\n");
		}
		lib_socket_print_buf(buf, len, "< ");
		//print_test_buf(buf, len);
		if (separate) {
			printf("\n");
		}
	}	

	buf[len] = 0;
	if (len < 14) { // prematured header
	    //printf("Header\n");
		lib_socket_close(fp->fd);
		fp->fd = -1;
		return -1;
	}

	ret = strtol(buf + 8, &p, 0); // HTTP return code
	if (ret == 200 && fp->offset > 0) { // 200 (complete result); then skip beginning of the file
	    //printf("Body\n");
		off_t rest = fp->offset;
		while (rest) {
			off_t l = rest < 0x10000? rest : 0x10000;
			rest -= _lib_nf_read((int) fp->fd, buf, l);
		}
	} else if (ret != 206 && ret != 200) {
		free(buf);
		fprintf(stderr, "[http_connect_file] fail to open file (HTTP code: %d).\n", ret);
		lib_socket_close(fp->fd);
		fp->fd = -1;
		return -1;
	}

	free(buf);
	fp->is_ready = 1;
	return 0;
}

int lib_nf_connect_file(lib_nf_file_t* fp) {
	if (!fp) {
		return -1;
	}

	if (fp->type == LIB_NF_TYPE_FTP) {
		return lib_nf_ftp_connect_file(fp);
	}

	if (fp->type == LIB_NF_TYPE_HTTP) {
		return lib_nf_http_connect_file(fp);
	}

	return -1;
}

lib_nf_file_t* lib_nf_parse_url(const char* url, const char* mode) {
	if (!url) {
		fprintf(stderr, "Cannot parse url. Url is NULL\n");
		return NULL;
	}

	lib_nf_file_t* fp = NULL;

	if (lib_nf_is_ftp_url(url)) {
		// FTP
		fp = lib_nf_ftp_parse_url(url, mode);
	} else if (lib_nf_is_http_url(url)) {
		// HTTP
		fp = lib_nf_http_parse_url(url, mode);
	} else {
		fprintf(stderr, "Cannot parse url: %s. Unsupported protocol\n", url);
	}

	return fp;
}

lib_nf_file_t* lib_nf_open(const char* url, const char* mode) {
	if (!url) {
		return NULL;
	}
	lib_nf_file_t* fp = NULL;

	if (lib_nf_is_ftp_url(url)) {
		// FTP
		fp = lib_nf_ftp_parse_url(url, mode);
		if (!fp) {
			return NULL;
		}
		lib_nf_ftp_connect_file(fp);
	} else if (lib_nf_is_http_url(url)) {
		// HTTP
		fp = lib_nf_http_parse_url(url, mode);
		if (!fp) {
			return NULL;
		}
		lib_nf_http_connect_file(fp);
	}

	if (fp && fp->fd == -1) {
		//net_close(fp);
		return NULL;
	}
	return fp;
}

char* lib_nf_get_file_contents_opts(lib_nf_file_t* fp, int* size, int init_size, int buf_size) {
	if (!fp) {
		return NULL;
	}
	int inc_size = 0;
	char* buf = (char*) calloc(init_size, 1);
	int bytes = 0;
	int total = 0;
	int allocated = init_size;

	//printf("init size     : %i\n",init_size);
	//printf("buffer size   : %i\n", buf_size);

    while ((bytes = lib_nf_read(fp, &buf[total], buf_size)) != 0) {
		//printf("read size     : %i\n", bytes);
	    if (total + bytes >= allocated) {
		    inc_size = bytes < buf_size ? bytes : buf_size;
		    allocated += inc_size;
		    //printf("increment size: %i\n", inc_size);
		    //printf("realloc size  : %i\n", allocated + 1);
		    buf = (char*) realloc(buf, (allocated + 1) * sizeof(char));
	    }
	    total += bytes;
    }

	buf[total] = '\0';

	//printf("total size    : %i\n", total);
	//printf("[n-2]         : %c\n", buf[total - 2]);
	//printf("[n-1]         : %i\n", buf[total - 1]);
	//printf("[n]           : %i\n", buf[total]);
	
	if (size) {
		*size = total;
	}
	return buf;
}

char* lib_nf_get_file_contents(lib_nf_file_t* fp, int* size) {
	return lib_nf_get_file_contents_opts(fp, size, 65536, 4096); // TOOD: Use constants
}

