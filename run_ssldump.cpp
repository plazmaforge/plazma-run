#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "arglib.h"
#include "iolib.h"

/*
Server Hello
=========================================================

Record Header       : 16 03 03 00 3d (61) - (handshake record)

Handshake Header    : 02 00 00 39    (57) - (server hello)
Server Version      : 03 03               - TLS 1.2
Server Random       : e6 aa 4c ef 26 14 21 f7 79 ce f7 ce cb 4d 47 89 6b 8d 23 84 25 c6 d9 82 44 4f 57 4e 47 52 44 01 
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

/*

Server Key Exchange
=========================================================

Record Header       : 16 03 03 01 2c

Handshake Header    : 0c 00 01 28     - (server key exchange)
Curve Info          : 03 00 1d        - ("curve x25519")

*/

void print_buf(const char* buf, size_t cur_pos, size_t data_len) {
    //printf("\ncur_pos: %lu, data_len: %lu\n", cur_pos, data_len);
    size_t i = 0;
    while (i < data_len) {
        printf("%02x", (unsigned char) buf[i + cur_pos]);
        if (i < data_len - 1) {
            printf(" ");
        }
        i++;
    }
}

void print_buf_line_ext(const char* name, const char* description, const char* buf, size_t cur_pos, size_t data_len) {
    printf("%s", name);
    print_buf(buf, cur_pos, data_len);
    if (description) {
        printf(" %s", description);
    }
    printf("\n");
}

void print_buf_line(const char* name, const char* buf, size_t cur_pos, size_t data_len) {
    print_buf_line_ext(name, NULL, buf, cur_pos, data_len); 
}

void print_buf_line_fix(const char* name, const char* buf, size_t cur_pos, size_t data_len, size_t fix_len) {
    // cur_pos              new_pos
    //    |        left_len    |          right_len  
    //-------------------------------------------------
    size_t rest_len = data_len - fix_len;
    size_t right_len = rest_len > fix_len ? fix_len : rest_len;
    size_t left_len = data_len - right_len;

    printf("%s", name);
    print_buf(buf, cur_pos, fix_len);
    printf(" ... ");
        
    print_buf(buf, cur_pos + left_len, right_len);
    printf("\n");
}

int check_buf_len(size_t cur_pos, size_t data_len, size_t buf_len) {
    if (cur_pos + data_len - 1 >= buf_len) {
        return 0;
    }
    return 1;
}

size_t to_size(const char* buf, size_t cur_pos) {
    char hex_str[4];
    sprintf(hex_str, "%02x%02x", (unsigned char) buf[cur_pos], (unsigned char) buf[cur_pos + 1]);
    unsigned long hex_val = strtoul(hex_str, NULL, 16);
    return (size_t) hex_val;
} 

//// Common: Client/server

int read_session_id(const char* buf, size_t buf_len, size_t& cur_pos) {

    // >> Session ID Length
    size_t data_len = 1;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    size_t session_id_len = buf[cur_pos];

    print_buf_line("Session ID Length   : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    if (session_id_len == 0) {
        return 0;
    }

    // >> Session ID
    data_len = session_id_len;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    print_buf_line("Session ID          : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    return 0;    
}

int read_cipher_suite(const char* buf, size_t buf_len, size_t& cur_pos) {

    // >> Cipher Suite    
    int data_len = 2;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    print_buf_line("Cipher Suite        : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    return 0;    
}

int read_compression_method(const char* buf, size_t buf_len, size_t& cur_pos) {
    int data_len = 1;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    print_buf_line("Compression Method  : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    return 0;    
}

////


int read_server_random(const char* buf, size_t buf_len, size_t& cur_pos) {
    int data_len = 32;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    print_buf_line("Server Random       : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    return 0;    
}

int read_server_session_id(const char* buf, size_t buf_len, size_t& cur_pos) {
    return read_session_id(buf, buf_len, cur_pos);
}

int read_server_cipher_suite(const char* buf, size_t buf_len, size_t& cur_pos) {
    return read_cipher_suite(buf, buf_len, cur_pos);
}

int read_server_compression_method(const char* buf, size_t buf_len, size_t& cur_pos) {
    return read_compression_method(buf, buf_len, cur_pos);
}

int read_server_extensions(const char* buf, size_t buf_len, size_t& cur_pos) {
    size_t data_len = 2;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    size_t extensions_len = to_size(buf, cur_pos);

    printf("Extensions Length   : (%lu)\n", extensions_len);

    print_buf_line("Extensions Length   : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    data_len = 5;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    size_t renegotiation_len = buf[cur_pos + 4]; // last byte
    printf("Renegotiation Len   : (%lu)\n", renegotiation_len);

    print_buf_line("Renegotiation Info  : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    if (extensions_len == 0) {
        return 0;
    }

    //data_len = 6; // go-OK
    data_len = extensions_len;

    // TODO: HACK
    if (renegotiation_len == 0) {
        // shift renegation info block
        data_len -= 5;
    }

    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    print_buf_line("Extensions          : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    return 0;    
}

////

int read_server_hello(const char* buf, size_t buf_len, size_t& cur_pos) {
    int data_len = 2;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    unsigned char version_1 = buf[cur_pos];
    unsigned char version_2 = buf[cur_pos + 1];

    print_buf_line("Server Version      : ", buf, cur_pos, data_len);
    cur_pos += data_len;
    int ret = 0;

    if (version_1 == 0x03 && version_2 == 0x03) {

        ret = read_server_random(buf, buf_len, cur_pos);
        if (ret != 0) {
            return ret;
        }

        ret = read_server_session_id(buf, buf_len, cur_pos);
        if (ret != 0) {
            return ret;
        }

        ret = read_server_cipher_suite(buf, buf_len, cur_pos);
        if (ret != 0) {
            return ret;
        }

        ret = read_server_compression_method(buf, buf_len, cur_pos);
        if (ret != 0) {
            return ret;
        }

        ret = read_server_extensions(buf, buf_len, cur_pos);
        if (ret != 0) {
            return ret;
        }

        return 0;
    } else {
        fprintf(stderr, "%s: Unsupported Server Version: %02x%02x", prog_name, version_1, version_2);
        return -1;
    }
    return 0;    
}

int read_server_certificate(const char* buf, size_t buf_len, size_t& cur_pos) {
    int data_len = 3;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    size_t certificates_len = to_size(buf, cur_pos + 1);
    printf("Certificates Length : (%lu)\n", certificates_len);
    print_buf_line("Certificates Length : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    data_len = 3;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }
    size_t certificate_len = to_size(buf, cur_pos + 1);
    printf("Certificate Length  : (%lu)\n", certificate_len);
    print_buf_line("Certificate Length  : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    size_t certificate_all_len = certificates_len - 3;
    size_t certificate_first_len = certificate_len;

    data_len = certificate_all_len;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    size_t fix_len = 16;
    if (data_len <= fix_len) {
        print_buf_line("Certificate         : ", buf, cur_pos, data_len);
    } else {
        print_buf_line_fix("Certificate         : ", buf, cur_pos, data_len, fix_len);
    }
    cur_pos += data_len;

    return 0;
}

int read_server_key_exchange(const char* buf, size_t buf_len, size_t& cur_pos) {

    // >> Curve Info
    int data_len = 3;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }    
    print_buf_line("Curve Info          : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    // >> Public Key Len
    data_len = 1;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }    
    size_t public_key_len = buf[cur_pos];
    printf("Public Key Len      : (%lu)\n", public_key_len);
    print_buf_line("Public Key Len      : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    if (public_key_len == 0) {
        return -1;
    }

    // >> Public Key
    data_len = public_key_len;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    print_buf_line("Public Key          : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    // >> Signature Value
    data_len = 2;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }    
    size_t signature_value = to_size(buf, cur_pos);
    printf("Signature Value     : (%lu)\n", signature_value);
    print_buf_line("Signature Value     : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    if (signature_value == 0) {
        return -1;
    }

    // >> Signature Len
    data_len = 2;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }    
    size_t signature_len = to_size(buf, cur_pos);
    printf("Signature Len       : (%lu)\n", signature_len);
    print_buf_line("Signature Len       : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    if (signature_len == 0) {
        return -1;
    }

    // >> Signature
    data_len = signature_len;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }    
    size_t fix_len = 16;
    if (data_len <= fix_len) {
        print_buf_line("Signature           : ", buf, cur_pos, data_len);
    } else {
        print_buf_line_fix("Signature           : ", buf, cur_pos, data_len, fix_len);
    }    
    cur_pos += data_len;

    //print_buf_line("Next.......         : ", buf, cur_pos, data_len);
    //cur_pos += data_len;
    //return -1;
    return 0;
}

int read_server_hello_done(const char* buf, size_t buf_len, size_t& cur_pos) {
    printf("Server Hello Done   : \n");
    return 0;
}

////

int read_handshake_header(const char* buf, size_t buf_len, size_t& cur_pos) {
    int data_len = 4;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    unsigned char handshake_type = buf[cur_pos];

    printf("\n");
    print_buf_line("Handshake Header    : ", buf, cur_pos, data_len);
    cur_pos += data_len;

    if (handshake_type == 0x02) {

        // Server Hello    
        return read_server_hello(buf, buf_len, cur_pos);
    } else if (handshake_type == 0x0b) {

        // Server Certificate
        return read_server_certificate(buf, buf_len, cur_pos);

    } else if (handshake_type == 0x0c) {

        // Server Key Exchange
        return read_server_key_exchange(buf, buf_len, cur_pos);
    } else if (handshake_type == 0x0e) {

        // Server Hello Done
        return read_server_hello_done(buf, buf_len, cur_pos);
    } else {
        fprintf(stderr, "%s: Unsupported Handshake Header: %02x\n", prog_name, handshake_type);
        return -1;
    }

    return 0;    
}

int read_record_header(const char* buf, size_t buf_len, size_t& cur_pos) {
    int data_len = 5;
    if (!check_buf_len(cur_pos, data_len, buf_len)) {
        return -1;
    }

    int first = cur_pos == 0;
    unsigned char record_type = buf[cur_pos];

    if (record_type == 0x16) {
        printf("\n");
        print_buf_line("Record Header       : ", buf, cur_pos, data_len);
        cur_pos += data_len;
    }

    if (record_type == 0x16 || record_type == 0x0b) {

        // Handshake Record
        return read_handshake_header(buf, buf_len, cur_pos);
    } else {
        fprintf(stderr, "%s: Unsupported Record Header: %02x\n", prog_name, record_type);
        return -1;
    }

    return -1;
}

int run_ssldump(const char* file_name) {

    if (!file_name) {
        fprintf(stderr, "%s: File name is empty\n", prog_name);
        return 1;
    }

    char* buf       = NULL;
    size_t buf_size = 0;

    ////
    int retval = lib_io_read_all_bytes(file_name, &buf, &buf_size);
    if (retval < 0) {
        // error
        fprintf(stderr, "%s: I/O Error\n", prog_name);
        free(buf);
        return 1;

    }
    ////

    if (buf_size == 0 || !buf) {
        // error
        fprintf(stderr, "%s: No Data\n", prog_name);
        free(buf);
        return 1;
    }

    size_t cur_pos = 0;
    while (cur_pos <= buf_size) {
        if (read_record_header(buf, buf_size, cur_pos) != 0) {
            fprintf(stderr, "%s: Read Error\n", prog_name);
            free(buf);
            return 1;
        }
    }
    return 0;
}

void usage() {
    printf("Usage: run-ssldump file\n");
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        usage();
        return 0;
    }

    prog_name = lib_arg_get_prog_name(argv);

    const char* file_name = argv[1];

    //lib_io_buf_init();
    
    return run_ssldump(file_name);
}

