#include <stdlib.h>
#include <stdio.h>

#include "socketlib.h"

void usage() {
    printf("Usage: run-get\n");
}

int main(int argc, char *argv[]) {

	char* buf;
	int type = 4; 
    int l;

// #ifdef _WIN32
// 	net_win32_init();
// #endif

	buf = (char*) calloc(0x100000, 1);

    nf_file_t* fp = http_parse_url("http://captive.apple.com:80/alaska.html", "r");
    //nf_file_t fp = net_open

    if (!fp) {
        fprintf(stderr, "Error parse url\n");
        return 1;
    }

    printf("host     : %s\n", fp->host);
    printf("port     : %i\n", fp->port);
    printf("path     : %s\n", fp->path);

    printf("http_host: %s\n", fp->http_host);

    if (http_connect_file(fp) != 0) {
        fprintf(stderr, "Error http connect\n");
        return 1;
    }

	net_read(fp, buf, 10000);
	//net_seek(fp, 20000, SEEK_SET);
	//net_seek(fp, 10000, SEEK_SET);
	//l = net_read(fp, buf+10000, 10000000) + 10000;

    printf("%s \n", buf);
}
