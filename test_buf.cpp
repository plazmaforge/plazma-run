#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <wchar.h>
#include <windows.h>
#endif

#define BUF_SIZE_MIN 4096
#define BUF_SIZE_DEF 4096

static int COUNT;
static int BUF_SIZE;

int _fwrite_v1(FILE* file) {

    const char *data = "This is a line of text.\n"; 
    size_t data_size = strlen(data); 

    // Write multiple lines in a loop
    for (int i = 0; i < COUNT; i++) { 
        fwrite(data, 1, data_size, file);
    } 

    fputs("The End: v1: fwrite\n", file);
    return 0;
} 

int _fwrite_v2(FILE* file) {

    // Write data
    for (int i = 0; i < COUNT; i++) {
        fprintf(file, "This is a line of %d.\n", i);
    }

    fputs("The End: v2: fprintf\n", file);
    return 0;
}

int _fwrite_v2b(FILE* file) {

    // Set a larger buffer size
    char buffer[BUF_SIZE];

    int retval = setvbuf(file, buffer, _IOFBF, sizeof(buffer)); 
    if (retval < 0) {
        return retval;
    }

    // Write data
    for (int i = 0; i < COUNT; i++) {
        fprintf(file, "This is a line of %d.\n", i);
    }

    fprintf(file, "The End: v2b: fprintf: buf_size=%d.\n", BUF_SIZE);
    //fputs("The End: v2b: fprintf: buf\n", file);
    return 0;
}

int _fwrite_v3(FILE* file) {

    const char *lines[COUNT]; 
    for (int i = 0; i < COUNT; i++) { 
        lines[i] = "This is a line of text.\n";
    }

    // Combine lines into a single buffer
    for (int i = 0; i < COUNT; i++) { 
        fputs(lines[i], file);
    } 

    fputs("The End: v3: fputs\n", file);
    return 0;
}

////

#ifdef _WIN32

wchar_t* _utf8_to_wcs(const char* str) {
    if (!str) {
        return NULL;
    }
    size_t len = strlen(str);
    int wlen = MultiByteToWideChar(CP_UTF8, 0, str, len, NULL, 0);
    //int wlen = len * 2;
    wchar_t* wstr = (wchar_t*) malloc(sizeof(wchar_t) * wlen);
    if (!wstr) {
        return NULL;
    }
    MultiByteToWideChar(CP_UTF8, 0, str, len, wstr, wlen);
    return wstr;
}

void _WriteConsoleW(const HANDLE handle, const wchar_t* str, size_t size, size_t* out_size) {
    if (!str) {
        return;
    }
    DWORD dsize;
    WriteConsoleW(handle, str, size, &dsize, NULL);
}

void _WriteConsole(const HANDLE handle, const char* str, size_t size, size_t* out_size) {
    if (!str) {
        return;
    }
    wchar_t* wstr = _utf8_to_wcs(str);
    size_t wsize = wcslen(wstr);
    size_t out_wsize;
    _WriteConsoleW(handle, wstr, wsize, &out_wsize);
}

// void _WriteConsole(const DWORD handleId, const std::wstring &buffer) {
//     const HANDLE handle = ::GetStdHandle(handleId);
//     _WriteConsole(handle, buffer);
// }

// void _WriteConsole(const DWORD handleId, const std::string &buffer) {
//     const HANDLE handle = ::GetStdHandle(handleId);
//     _WriteConsole(handle, buffer);
// }

// void _WriteConsoleOutput(const std::wstring &buffer) {
//     _WriteConsole(STD_OUTPUT_HANDLE, buffer);
// }

int _win_fputs(const char* str) {
    if (!str) {
        return 0;
    }
    size_t size = strlen(str);
    size_t out_size;
    _WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), str, size, &out_size);
    return 0;
}

#endif

int _fwrite_v3w(FILE* file) {

    // Set a larger buffer size
    char buffer[BUF_SIZE];

    int retval = setvbuf(file, buffer, _IOFBF, sizeof(buffer)); 
    if (retval < 0) {
        return retval;
    }

    #ifdef _WIN32
    //if (file != stdout) {
    //    return 0;
    //}

    fprintf(stderr, "_fwrite_v3w: start\n");

    // Combine lines into a single buffer
    for (int i = 0; i < COUNT; i++) { 
        _win_fputs("This is a line of text.\n");
    } 

    fprintf(stderr, "_fwrite_v3w: end\n");

    _win_fputs("The End: v3w\n");
    #else
    fputs("The End: v3w: Unsupported\n", file);
    #endif
    return 0;
}

////

int _write_to_stream(int ver, FILE* file) {
    if (ver == 1) {
        return _fwrite_v1(file);
    }
    if (ver == 2) {
        return _fwrite_v2(file);
    }
    if (ver == 22) {
        return _fwrite_v2b(file);
    }
    if (ver == 3) {
        return _fwrite_v3(file);
    }
    if (ver == 33) {
        return _fwrite_v3w(file);
    }

    // default
    return _fwrite_v1(file);;
}

int _write_to_file(int ver, const char* file_name) {
    FILE *file = fopen(file_name, "wb");

    if (!file) { 
        perror("Failed to open file"); 
        return 1;
    } 

    int retval = _write_to_stream(ver, file);
    fclose(file);
    return retval;    
} 

int main(int argc, char* argv[]) {

    BUF_SIZE = BUF_SIZE_DEF; // 4 KB buffer
    COUNT    = 100000;

    int ver = 1;
    if (argc > 1) {
        ver = atoi(argv[1]);
    }
    if (argc > 2) {
        BUF_SIZE = atoi(argv[2]);
        if (BUF_SIZE < BUF_SIZE_MIN) {
            BUF_SIZE = BUF_SIZE_MIN;
        }
    }
    _write_to_stream(ver, stdout);
    return 0;
}
