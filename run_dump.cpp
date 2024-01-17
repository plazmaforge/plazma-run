
#include <stdio.h>

#include "iolib.h"

void printUsage() {
    printf("Usage: run-dump file\n");
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printUsage();
        return 0;
    }

    char* fileName = argv[1];
    dumpFile(fileName);
}