#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "dumplib.h"

/*

Server Hello
=========================================================

Record Header       : 16 03 03 00 3d (61) - (handshake record)

Handshake Header    : 02 00 00 39    (57) - (server hello)

Server Version      : 03 03               - TLS 1.2

Server Random:      : e6 aa 4c ef 26 14 21 f7 79 ce f7 ce cb 4d 47 89 6b 8d 23 84 25 c6 d9 82 44 4f 57 4e 47 52 44 01 
 
Session ID Length   : 00

Cipher Suite        : c0 2b               -  TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256

Compression Method  : 00

Extensions Length   : 00 11           (17)

Renegotiation Info  : ff 01 00 01 00 

Extensions          : 00 00 00 00 00 0b 00 04 03 00 01 02
*/

/*
Server Certificate
=========================================================

Record Header       : 16 03 03 0c 3a  (3130)  - (handshake record)

Handshake Header    : 0b 00 0c 36     (3126)  - (certificate)

Certificates Length : 00 0c 33        (3123) 
Certificates Length : 00 04 a7        (1191)

*/

void usage() {
    printf("Usage: run-ssl-dump <file>\n");
}

int print_buf(const char* buf, size_t cur_pos, size_t buf_len) {
    //printf("\ncur_pos: %lu, buf_len: %lu\n", cur_pos, buf_len);
    size_t i = 0;
    while (i < buf_len) {
        printf("%02x", (unsigned char) buf[i + cur_pos]);
        if (i < buf_len - 1) {
            printf(" ");
        }
        i++;
    }
    printf("\n");

    return 0;
}

int check_buf_len(size_t cur_pos, size_t buf_len, size_t data_len) {
    if (cur_pos + buf_len - 1 >= data_len) {
        return 0;
    }
    return 1;
}

size_t to_size(const char* data, size_t cur_pos) {
    char hex_str[4];
    sprintf(hex_str, "%02x%02x", (unsigned char) data[cur_pos], (unsigned char) data[cur_pos + 1]);
    unsigned long hex_val = strtoul(hex_str, NULL, 16);
    return (size_t) hex_val;
} 

//// Common: Client/server

int read_session_id(const char* data, size_t& cur_pos, size_t data_len) {
    size_t buf_len = 1;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    unsigned char session_id = data[cur_pos];

    printf("Session ID Length   : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    if (session_id == 0) {
        return 0;
    }

    buf_len = session_id;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    printf("Session ID          : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    return 0;    
}

int read_cipher_suite(const char* data, size_t& cur_pos, size_t data_len) {
    int buf_len = 2;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    printf("Cipher Suite        : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    return 0;    
}

int read_compression_method(const char* data, size_t& cur_pos, size_t data_len) {
    int buf_len = 1;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    printf("Compression Method  : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    return 0;    
}

////


int read_server_random(const char* data, size_t& cur_pos, size_t data_len) {
    int buf_len = 32;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    printf("Server Random       : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    return 0;    
}

int read_server_session_id(const char* data, size_t& cur_pos, size_t data_len) {
    return read_session_id(data, cur_pos, data_len);
}

int read_server_cipher_suite(const char* data, size_t& cur_pos, size_t data_len) {
    return read_cipher_suite(data, cur_pos, data_len);
}

int read_server_compression_method(const char* data, size_t& cur_pos, size_t data_len) {
    return read_compression_method(data, cur_pos, data_len);
}

int read_server_extensions(const char* data, size_t& cur_pos, size_t data_len) {
    size_t buf_len = 2;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    size_t extensions_len = to_size(data, cur_pos);

    printf("Extensions Length   : (%lu)\n", extensions_len);

    printf("Extensions Length   : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    buf_len = 5;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    printf("Renegotiation Info  : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    if (extensions_len == 0) {
        return 0;
    }

    buf_len = extensions_len;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    printf("Extensions          : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    return 0;    
}

////

int read_server_hello(const char* data, size_t& cur_pos, size_t data_len) {
    int buf_len = 2;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    unsigned char version_1 = data[cur_pos];
    unsigned char version_2 = data[cur_pos + 1];

    printf("Server Version      : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;
    int ret = 0;

    if (version_1 == 0x03 && version_2 == 0x03) {

        ret = read_server_random(data, cur_pos, data_len);
        if (ret != 0) {
            return ret;
        }

        ret = read_server_session_id(data, cur_pos, data_len);
        if (ret != 0) {
            return ret;
        }

        ret = read_server_cipher_suite(data, cur_pos, data_len);
        if (ret != 0) {
            return ret;
        }

        ret = read_server_compression_method(data, cur_pos, data_len);
        if (ret != 0) {
            return ret;
        }

        ret = read_server_extensions(data, cur_pos, data_len);
        if (ret != 0) {
            return ret;
        }

        return 0;
    } else {
        fprintf(stderr, "Unsupported Server Version: %02x%02x", version_1, version_2);
        return -1;
    }
    return 0;    
}

int read_server_certificate(const char* data, size_t& cur_pos, size_t data_len) {
    int buf_len = 3;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    size_t certificates_len = to_size(data, cur_pos + 1);
    printf("Certificates Length : (%lu)\n", certificates_len);
    printf("Certificates Length : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    buf_len = 3;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }
    size_t certificate_len = to_size(data, cur_pos + 1);
    printf("Certificate Length  : (%lu)\n", certificate_len);
    printf("Certificate Length  : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    size_t certificate_all_len = certificates_len - 3;
    size_t certificate_first_len = certificate_len;

    buf_len = certificate_all_len;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    printf("Certificate         : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    ////

    buf_len = 9;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    printf("Next.......         : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    return -1;
}

int read_handshake_header(const char* data, size_t& cur_pos, size_t data_len) {
    int buf_len = 4;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    unsigned char handshake_type = data[cur_pos];

    printf("Handshake Header    : ");
    print_buf(data, cur_pos, buf_len);
    cur_pos += buf_len;

    if (handshake_type == 0x02) {

        // Server Hello    
        return read_server_hello(data, cur_pos, data_len);
    } else if (handshake_type == 0x0b) {

        // Server Certificate
        return read_server_certificate(data, cur_pos, data_len);
    } else {
        fprintf(stderr, "Unsupported Handshake Header: %02x", handshake_type);
        return -1;
    }

    return 0;    
}

int read_record_header(const char* data, size_t& cur_pos, size_t data_len) {
    int buf_len = 5;
    if (!check_buf_len(cur_pos, buf_len, data_len)) {
        return -1;
    }

    int first = cur_pos == 0;
    unsigned char record_type = data[cur_pos];

    if (record_type == 0x16) {
        printf("Record Header       : ");
        print_buf(data, cur_pos, buf_len);
        cur_pos += buf_len;
    }

    if (record_type == 0x16 || record_type == 0x0b) {

        // Handshake Record
        return read_handshake_header(data, cur_pos, data_len);
    } else {
        fprintf(stderr, "Unsupported Record Header: %02x\n", record_type);
        return -1;
    }

    return -1;
}

int run_ssl_dump(const char* file_name) {

    if (!file_name) {
        fprintf(stderr, "File name is empty\n");
        return 1;
    }

    size_t size = 0;
    char* data = read_bytes_size(file_name, size);
    if (size == 0 || !data) {
        fprintf(stderr, "No input data\n");
        return 1;
    }

    size_t cur_pos = 0;
    while (cur_pos <= size) {
        if (read_record_header(data, cur_pos, size) != 0) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        usage();
        return 0;
    }

    const char* file_name = argv[1];
    
    #ifdef _WIN32
    char buf[65536];
    setvbuf(stdout, buf, _IOFBF, 65536);
    #endif

    run_ssl_dump(file_name);
}

