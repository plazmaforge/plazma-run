
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <ctype.h>

#include "iolib.h"

// https://www.delftstack.com/howto/c/read-binary-file-in-c/

char* lib_io_read_bytes(const char* file_name) {
    size_t size = 0;
    return lib_io_read_bytes_size(file_name, size);
}

char* lib_io_read_bytes_size(const char* file_name, size_t& size) {

  // Read
  FILE* in_file = fopen(file_name, "rb");
  if (!in_file) {
    perror("fopen");
    return NULL;
    //exit(EXIT_FAILURE);
  }

  struct stat sb;
  if (stat(file_name, &sb) == -1) {
    perror("stat");
    return NULL;
    //exit(EXIT_FAILURE);
  }
  
  if (size <= 0 || size > sb.st_size) {
    size = sb.st_size;
  }

  char* data = (char*) malloc(size * sizeof(char));
  //fread(data, sb.st_size, 1, in_file);
  fread(data, size, 1, in_file);
  //printf("read data: size = %lu\n", size);

  fclose(in_file);

  //exit(EXIT_SUCCESS);

  return data;

}

void lib_io_write_bytes_size(const char* file_name, char* data, size_t& size) {

  // Write
  FILE* output_file = fopen(file_name, "wb+");
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

