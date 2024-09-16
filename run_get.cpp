#include <stdlib.h>
#include <stdio.h>

#include "socketlib.h"
#include "nflib.h"

void usage() {
    fprintf(stderr, "Usage: run-get url\n");
}

int run_get(const char* url) {

    if (!url) {
        fprintf(stderr, "Cannot parse url. Url is empty\n");
        return 1;
    }

// #ifdef _WIN32
// 	nf_win32_init();
// #endif

    // "http://captive.apple.com"
    //printf(">> url: %s\n", url);

    lib_nf_file_t* fp = lib_nf_parse_url(url, NULL);
    if (!fp) {
        return 1;
    }

    printf("* host     : %s\n", fp->host);
    printf("* port     : %i\n", fp->port);
    printf("* path     : %s\n", fp->path);
    printf("* http_host: %s\n", fp->http_host);

    if (lib_nf_connect_file(fp) != 0) {
        return 1;
    }

    int size = 0;
    char* buf = lib_nf_get_file_contents(fp, &size);
    printf("%s", buf);

    //int len = 0;
    //int buf_len = 20;

    //printf("len: %i, ready: %i\n", len, fp->is_ready);
    //printf("starting...\n");

    //while ((len = lib_nf_read(fp, buf, buf_len)) != 0) {
       //len = lib_nf_read(fp, buf, buf_len);
       //printf("len: %i, ready: %i\n", len, fp->is_ready);
       //printf("%s", buf);
    //}

    //len = lib_nf_read(fp, buf, 10000);
    // printf("len: %i, ready: %i", len, fp->is_ready);
	//lib_nf_read(fp, buf, 10000);
	//lib_nf_seek(fp, 20000, SEEK_SET);
	//lib_nf_seek(fp, 10000, SEEK_SET);
	//len = lib_nf_read(fp, buf+10000, 10000000) + 10000;

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage();
        return 0;
    }
    const char* url = argv[1];

    #ifdef _WIN32
    char buf[65536];
    setvbuf(stdout, buf, _IOFBF, 65536);
    #endif

    return run_get(url);
}
