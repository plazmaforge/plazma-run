#include <stdlib.h>
#include <stdio.h>

#include "socketlib.h"
#include "netlib.h"

void usage() {
    printf("Usage: run-get <url>\n");
}

int run(const char* url) {

    if (!url) {
        return 1;
    }

// #ifdef _WIN32
// 	net_win32_init();
// #endif

    // "http://captive.apple.com"

    nf_file_t* fp = net_parse_url(url, NULL);
    if (!fp) {
        return 1;
    }

    printf("* host     : %s\n", fp->host);
    printf("* port     : %i\n", fp->port);
    printf("* path     : %s\n", fp->path);
    printf("* http_host: %s\n", fp->http_host);

    if (net_connect_file(fp) != 0) {
        return 1;
    }

    int size = 0;
    char* buf = net_get_file_contents(fp, &size);
    printf("%s", buf);

    //int len = 0;
    //int buf_len = 20;

    //printf("len: %i, ready: %i\n", len, fp->is_ready);
    //printf("starting...\n");

    //while ((len = net_read(fp, buf, buf_len)) != 0) {
       //len = net_read(fp, buf, buf_len);
       //printf("len: %i, ready: %i\n", len, fp->is_ready);
       //printf("%s", buf);
    //}

    //len = net_read(fp, buf, 10000);
    // printf("len: %i, ready: %i", len, fp->is_ready);
	//net_read(fp, buf, 10000);
	//net_seek(fp, 20000, SEEK_SET);
	//net_seek(fp, 10000, SEEK_SET);
	//len = net_read(fp, buf+10000, 10000000) + 10000;

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage();
        return 0;
    }
    const char* url = argv[1];
    return run(url);
}
