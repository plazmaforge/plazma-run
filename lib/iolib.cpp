
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

void lib_fs_file_data_free(fs_file_data_t** file_list, int file_count) {
    if (!file_list || file_count <= 0) {
        return;
    }
    for (int i = 0; i < file_count; i++) {
        free(file_list[i]);
    }
    free(file_list);
}

char* lib_io_read_cat_bytes(const char** file_names, int file_count, size_t& size) {

    fs_file_data_t** file_list = (fs_file_data_t**) malloc(sizeof(fs_file_data_t) * file_count);

    fs_file_data_t* file_data = NULL;
    const char* file_name = NULL;
    size_t file_size = 0;
    size_t total_size = 0;

    for (int i = 0; i < file_count; i++) {
        file_name = file_names[i];
        file_size = 0;

        file_data = (fs_file_data_t*) malloc(sizeof(fs_file_data_t));
        file_data->data = lib_io_read_bytes_size(file_name, file_size);
        file_data->size = file_size;

        file_list[i] = file_data;
        total_size += file_size;

    }

    // V1
    char* data = (char*) malloc((total_size + 1) * sizeof(char));
    int offset = 0;

    for (int j = 0; j < file_count; j++) {

        file_data = file_list[j];
        
        for (int i = 0; i < file_data->size; i++) {
            data[i + offset] = file_data->data[i];
        }

        offset += file_data->size;

    }

    lib_fs_file_data_free(file_list, file_count);

    data[total_size] = '\0';
    size = total_size;

    return data;

}

