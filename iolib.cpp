
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cctype>

#include "iolib.h"

// https://www.delftstack.com/howto/c/read-binary-file-in-c/

char* readBytes(const char* fileName) {
    size_t size = 0;
    return readBytes(fileName, size);
}

char* readBytes(const char* fileName, size_t& size) {

  // Read
  FILE* in_file = fopen(fileName, "rb");
  if (!in_file) {
    perror("fopen");
    return NULL;
    //exit(EXIT_FAILURE);
  }

  struct stat sb;
  if (stat(fileName, &sb) == -1) {
    perror("stat");
    return NULL;
    //exit(EXIT_FAILURE);
  }
  
  if (size <= 0 || size > sb.st_size) {
    size = sb.st_size;
  }

  char* data = (char*) malloc(size);
  //fread(data, sb.st_size, 1, in_file);
  fread(data, size, 1, in_file);
  //printf("read data: size = %lu\n", size);

  fclose(in_file);

  //exit(EXIT_SUCCESS);

  return data;

}

void writeBytes(const char* fileName, char* data, size_t& size) {

  // Write
  FILE* output_file = fopen(fileName, "wb+");
  if (!output_file) {
    perror("fopen");
    return;
    //exit(EXIT_FAILURE);
  }

  //size_t size = sizeof data;

  fwrite(data, 1, size, output_file);
  //printf("write data: size = %lu\n", size);
  fclose(output_file);

  // Read
  // int fd = open(fileName, O_RDONLY);
  // if (fd == -1) {
  //   perror("open\n");
  //   exit(EXIT_FAILURE);
  // }

  // struct stat sb;
  // if (stat(fileName, &sb) == -1) {
  //   perror("stat");
  //   exit(EXIT_FAILURE);
  // }

  // char* file_contents = (char*) malloc(sb.st_size);
  // read(fd, file_contents, sb.st_size);

  // printf("read data: %s\n", file_contents);
  // close(fd);

  // free(file_contents);

  // exit(EXIT_SUCCESS);
}

void dumpBytes(const char* data, size_t size) {
    if (data == NULL || size == 0) {
        return;
    }
    int col_count = 32; //16;
    int row_count = size / col_count;
    if (size % col_count > 0) {
        row_count++;
    }

    bool showText = true;
    int offset = 0;
    int i = 0;
    for (int row = 0; row < row_count; row++) {
        offset = row * col_count;
        printf("0x%06X: ", (unsigned int) offset);

        // print data
        for (int col = 0; col < col_count; col++) {
            i = col + offset;
            if (i == size) {
                break;
            }
            printf(" %02X", (unsigned char) data[i]);
            //printf("%02hhx", data[i]);
            //printf("%02HHx", data[i]);
        }

        if (!showText) {
            printf("\n");
            break;
        }

        // data/text separator
        printf(" ");

        // last row:: fill rest size '___'
        int reserve_size = col_count * row_count;
        int rest_size = reserve_size - size;
        if (row == row_count - 1 &&  rest_size > 0) {
            for (int i = 0 ; i < rest_size; i++) {
                printf("   "); // _XX
            }
        }

        // print text
        for (int col = 0; col < col_count; col++) {
            i = col + offset;
            if (i == size) {
                break;
            }
            printf("%c", std::isprint(data[i]) ? data[i] : '.');
        }

        printf("\n");

    }

    /*
    for (int i = 0; i < size; i++) {
          printf(" %02X", (unsigned char) data[i]);
          //printf("%02hhx", data[i]);
          //printf("%02HHx", data[i]);
    }*/

}

void dumpFile(const char* fileName) {
    size_t size = 0;
    dumpFile(fileName, size);
}

void dumpFile(const char* fileName, size_t& size) {

  char* data = readBytes(fileName, size);

  //printf("HEX: file=%s, size=%lu\n", fileName, size);
  dumpBytes(data, size);
  free(data);
  
}
