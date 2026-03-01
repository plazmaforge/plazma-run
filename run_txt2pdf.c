#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "clilib.h"
#include "iolib.h"
#include "run_pdflib.h"

int run_txt2pdf_data(run_pdf_config_t* config, char* data, size_t size) {
    return run_pdf(config, data, size);    
}

int run_txt2pdf_file(run_pdf_config_t* config, const char* file_name) {
    
    if (!file_name) {
        fprintf(stderr, "%s: File name is empty\n", prog_name);
        return 1;
    }

    char* data  = NULL;
    size_t size = 0;
    int error = lib_io_read_all_bytes(file_name, &data, &size);
    if (error) {
        fprintf(stderr, "%s: %s: No such file or directory\n", prog_name, file_name);
        if (data) {
            free(data);
        }        
        return 1;
    }
    error = run_txt2pdf_data(config, data, size);
    if (data) {
        free(data);
    }
    return error;    
}

void usage() {
    fprintf(stderr, "Usage: run-txt2pdf -s <string> | <file>\n");
}

int main(int argc, char* argv[]) {

    lib_cli_prog_init(argv);

    if (lib_cli_not_arg(argc)) {
        usage();
        return 1;
    }

    int error = 0;
    int opt;
    int long_ind;

    bool flag_string            = false;
    char* data                  = NULL;
    size_t size                 = 0;
    const char* file_name       = NULL;
    const char* out_file_name   = NULL;

    // config
    const char* encoding        = NULL;
    const char* title           = NULL;
    const char* margin          = NULL;
    const char* font_name       = NULL;
    const char* font_style      = NULL;
    const char* font_weight     = NULL;
    const char* font_size       = NULL;

    bool flag_encoding          = false;
    bool flag_title             = false;
    bool flag_margin            = false;
    bool flag_font_name         = false;
    bool flag_font_style        = false;
    bool flag_font_weight       = false;
    bool flag_font_size         = false;
    bool flag_unicode           = false;

    static lib_option long_options[] = {
        {LIB_OPT_ENCODING,    optional_argument, 0, LIB_OPT_ENCODING_ID},
        {LIB_OPT_TITLE,       optional_argument, 0, LIB_OPT_TITLE_ID},
        {LIB_OPT_MARGIN,      optional_argument, 0, LIB_OPT_MARGIN_ID},
        {LIB_OPT_FONT_NAME,   optional_argument, 0, LIB_OPT_FONT_NAME_ID},
        {LIB_OPT_FONT_STYLE,  optional_argument, 0, LIB_OPT_FONT_STYLE_ID},
        {LIB_OPT_FONT_WEIGHT, optional_argument, 0, LIB_OPT_FONT_WEIGHT_ID},
        {LIB_OPT_FONT_SIZE,   optional_argument, 0, LIB_OPT_FONT_SIZE_ID},
        {LIB_OPT_UNICODE,     no_argument, 0, LIB_OPT_UNICODE_ID},
        {NULL,                0,                 0, 0 }
    };

    while ((opt = lib_getopt_long(argc, argv, "s:o:", long_options, &long_ind)) != -1) {

        switch (opt) {
        case 's':
            flag_string = true;
            data = optarg;
            break;
        case 'o':
            out_file_name = optarg;
            break;
        case LIB_OPT_CHARSET_ID:     // encoding
            flag_encoding = true;
            encoding = optarg;
            break;
        case LIB_OPT_TITLE_ID:       // title
            flag_title = true;
            title = optarg;
            break;
        case LIB_OPT_MARGIN_ID:      // margin
            flag_margin = true;
            margin = optarg;
            break;
        case LIB_OPT_FONT_NAME_ID:   // font-name
            flag_font_name = true;
            font_name = optarg;
            break;
        case LIB_OPT_FONT_STYLE_ID:  // font-style
            flag_font_style = true;
            font_style = optarg;
            break;
        case LIB_OPT_FONT_WEIGHT_ID: // font-weight
            flag_font_weight = true;
            font_weight = optarg;
            break;
        case LIB_OPT_FONT_SIZE_ID:   // font-size
            flag_font_size = true;
            font_size = optarg;
            break;
        case LIB_OPT_UNICODE_ID:     // unicode
            flag_unicode = true;
            break;
        case '?':
            error = 1;
            break;
        case ':':
            error = 1;
            break;
        }
    }

    if (error) {
        usage();
        return 1;
    }

    if (flag_string) {
        optind--;
    }

    if (lib_cli_not_argmin(argc, optind, 1)) {
        fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
        usage();
        return 1;
    }

    error = 0;
    if (flag_string) {
        //BY_STRING
        if (!data) {
            fprintf(stderr, "%s: Input string is required\n", prog_name);
            error = 1;
        } else {
            size = strlen(data);
            if (size == 0) {
                fprintf(stderr, "%s: Input string is empty\n", prog_name);
                error = 1;
            }
        }
    } else {
        //BY_FILE
        if (lib_cli_not_argind(argc, optind)) {
            fprintf(stderr, "%s: File name is required\n", prog_name);
            error = 1;
        } else {
            file_name = argv[optind];
        }
    }

    if (error) {
        usage();
        return 1;
    }

    run_pdf_config_t config;
    lib_pdf_init(&config);

    config.encoding    = lib_ifs(flag_encoding, encoding, LIB_PDF_ENCODING);
    config.encoding_id = lib_enc_get_conv_encoding_id(encoding);
    config.title       = lib_ifs(flag_title, title, LIB_PDF_TITLE);
    config.margin      = lib_ifs(flag_margin, margin, LIB_PDF_MARGIN);

    font_name   = lib_ifs(flag_font_name, font_name, LIB_PDF_FONT_NAME);
    font_style  = lib_ifs(flag_font_style, font_style, LIB_PDF_FONT_STYLE);
    font_weight = lib_ifs(flag_font_weight, font_weight, LIB_PDF_FONT_WEIGHT);
    font_size   = lib_ifs(flag_font_size, font_size, LIB_PDF_FONT_SIZE);

    if (font_name || font_style || font_weight || font_size) {
        lib_font_t font;
        config.font = &font;
        lib_doc_font_init(&font, font_name, font_style, font_weight, font_size);
    }
    config.use_unicode = flag_unicode;

    config.out_file_name = out_file_name;
    error = lib_run_out_open((run_doc_config_t*) &config);

    if (error) {
        return 1;
    }

     if (flag_string) {
        error = run_txt2pdf_data(&config, data, size);
    } else {
        error = run_txt2pdf_file(&config, file_name);
    }

    lib_run_out_close((run_doc_config_t*) &config);

    if (error) {
        return 1;
    }
    
    return 0;
}