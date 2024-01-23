
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "iolib.h"
#include "wstrlib.h"
#include "syslib.h"

struct FileData {
    char* data;
    size_t size;
};

void printUsage() {
    printf("Usage: run-cat <file...>\n");
}

int main(int argc, char* argv[]) {

    //setlocale(LC_ALL, "");
    init_locale(); // WIN32 fast output with setvbuf(?)

    if (argc < 2) {
        printUsage();
        return 0;
    }

    int fileCount = argc - 1;

    FileData** fileList = (FileData**) malloc(sizeof(FileData) * fileCount);
    //int* fileSize = (int*) malloc(file_count);

    char* fileName = NULL;
    FileData* fileData = NULL;
    char* allData = NULL;
    size_t fileSize = 0;

    size_t totalSize = 0;

    for (int i = 1; i < argc; i++) {
        fileName = argv[i];
        fileSize = 0;

        fileData = new FileData();
        fileData->data = readBytes(fileName, fileSize);
        fileData->size = fileSize;

        fileList[i - 1] = fileData;
        totalSize += fileSize; 
    }

    // V1
    char* totalData = (char*) malloc(totalSize);
    int offset = 0;

    for (int j = 0; j < fileCount; j++) {

        fileData = fileList[j];
        
        for (int i = 0; i < fileData->size; i++) {
            totalData[i + offset] = fileData->data[i];
        }

        offset += fileData->size;

    }

    #ifdef _WIN32
    char buf[65536];
    setvbuf(stdout, buf, _IOFBF, 65536);
    #endif

    // incorrect last chars: non printable char
    //printf("%s", totalData);
    //printf("%ls", char2wchar(totalData));   // IMPORTANT for WIN32 
    //wprintf(L"%ls", char2wchar(totalData)); // IMPORTANT for WIN32

 
    for (int i = 0; i < totalSize; i++) {                                            
        printf("%c", totalData[i]);
    }

    //char* fileName = argv[1];
    //hexDump(fileName);

    //reset_locale();
}