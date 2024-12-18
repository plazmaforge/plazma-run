#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "config.h"
#include "iolib.h"

/**
 * Allocate char array
 */
static char* _char_new(size_t size) {
  return (char*) calloc(size, sizeof(char));
}

/**
 * Allocate NUL-terminated data array
 */
static char* _data_new(size_t size) {
  char* data = _char_new(size + 1);
  if (!data) {
    return NULL;
  }
  data[size] = '\0'; /* NUL-terminated */
  return data;
}

static long _file_size_stat(const char* file_name) {
  if (!file_name) {
    return -1;
  }
  struct stat sb;
  if (stat(file_name, &sb) == -1) {
    return -1;
  }
  return sb.st_size; 
}

static long _file_size_fstat(int fd) {
  if (fd < 0) {
    return -1;
  }
  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    return -1;
  }
  return sb.st_size; 
}

static long _file_size_fseek(FILE* file) {
  if (!file) {
    return -1;
  }  
  if (fseek(file, 0, SEEK_END) != 0) {
    rewind(file);
    //fclose(file);
    return -1;
  }

  long size = ftell(file);
  rewind(file);
  //fclose(file);

  //if (size == LONG_MAX) {
  //  size = -1;
  //}  
  return size;
}

static long _file_size_seek(const char* file_name) {
  if (!file_name) {
    return -1;
  }
  FILE* file = fopen(file_name, "rb");
  if (!file) {
    return -1;
  }
  return _file_size_fseek(file);
}

static void _lib_io_file_data_init(lib_fs_file_data_t* file_data) {
  if (!file_data) {
    return;
  }
  file_data->data = NULL;
  file_data->size = 0;
}

static lib_fs_file_data_t* _lib_io_file_data_new() {
  lib_fs_file_data_t* file_data = (lib_fs_file_data_t*) malloc(sizeof(lib_fs_file_data_t));
  if (!file_data) {
    return NULL;
  }
  _lib_io_file_data_init(file_data);
  return file_data;
}

static lib_fs_file_data_t** _lib_io_file_list_new(size_t size) {
  return (lib_fs_file_data_t**) calloc(size, sizeof(lib_fs_file_data_t*));
}

////

int _lib_io_read_bytes(const char* file_name, char** data, size_t size) {

  #ifdef LIB_DEBUG
  fprintf(stderr, ">> io_read_bytes: size=%lu\n", size);
  #endif

  if (!file_name || !data) {
    // error: args
    #ifdef LIB_ERROR
    if (!file_name) {
      fprintf(stderr, "ERROR: Invalid arguments: file_name\n");
    }
    if (!data) {
      fprintf(stderr, "ERROR: Invalid arguments: data\n");
    }    
    #endif     
    return -1;
  }

  // Read
  FILE* file = fopen(file_name, "rb");
  if (!file) {
    // error: io
    #ifdef LIB_ERROR
    fprintf(stderr, "ERROR: fopen: file_name=%s\n", file_name);
    #endif     
    return -1;
  }

  size_t file_size = 0;
  if ((file_size = _file_size_stat(file_name)) < 0) {
  //if ((file_size = _file_size_fseek(file)) < 0) {  
    // error: io
    #ifdef LIB_ERROR
    fprintf(stderr, "ERROR: file_size: file_name=%s\n", file_name);
    #endif     
    return -1;
  }

  if (size == 0 || size > file_size) {
    size = file_size;
  }

  #ifdef LIB_DEBUG
  fprintf(stderr, ">> io_read_bytes: open: file_size=%lu\n", file_size);
  #endif

  char* _data = _data_new(size);
  if (!_data) {
    // error: mem
    return -1;
  }

  size_t _size = fread(_data, sizeof(char), size, file);

  #ifdef LIB_DEBUG
  fprintf(stderr, ">> io_read_bytes: read: size=%lu\n", _size);
  #endif

  _data[_size] = '\0'; /* NUL-terminated */
  *data = _data;

  fclose(file);  

  #ifdef LIB_DEBUG
  fprintf(stderr, ">> io_read_bytes: close\n");
  #endif

  return _size;

}

int _lib_io_write_bytes(const char* file_name, char* data, size_t size) {

  #ifdef LIB_DEBUG
  fprintf(stderr, ">> io_write_bytes: size=%lu\n", size);
  #endif

  if (!file_name || !data) {
    // error: args
    #ifdef LIB_ERROR
    if (!file_name) {
      fprintf(stderr, "ERROR: Invalid arguments: file_name\n");
    }
    if (!data) {
      fprintf(stderr, "ERROR: Invalid arguments: data\n");
    }
    #endif    
    return -1;
  }

  if (size == 0) {
    // no data
    #ifdef LIB_DEBUG
    fprintf(stderr, ">> io_write_bytes: return: size=%lu\n", size);
    #endif
    return 0;
  }

  // Write
  FILE* file = fopen(file_name, "wb+");
  if (!file) {
    // error: io
    #ifdef LIB_ERROR
    fprintf(stderr, "ERROR: fopen: file_name=%s\n", file_name);
    #endif     
    return -1;
  }

  #ifdef LIB_DEBUG
  fprintf(stderr, ">> io_write_bytes: open: input_size=%lu\n", size);
  #endif

  size_t _size = fwrite(data, sizeof(char), size, file);

  #ifdef LIB_DEBUG
  fprintf(stderr, ">> io_write_bytes: write: size=%lu\n", _size);
  #endif

  fclose(file);

  #ifdef LIB_DEBUG
  fprintf(stderr, ">> io_write_bytes: close\n");
  #endif

  return _size;
}

////

int lib_io_read_all_bytes(const char* file_name, char** data) {
  // (!): size = 0: for load all data 
  return _lib_io_read_bytes(file_name, data, 0);
}

int lib_io_read_bytes(const char* file_name, char** data, size_t size) {
  // (!): size = 0: no load
  if (size == 0) {
    return 0;
  }
  return _lib_io_read_bytes(file_name, data, size);
}

int lib_io_write_all_bytes(const char* file_name, char* data, size_t size) {
  return _lib_io_write_bytes(file_name, data, size);
}

int lib_io_write_bytes(const char* file_name, char* data, size_t size) {
  return _lib_io_write_bytes(file_name, data, size);
}

////

void lib_fs_file_data_free(lib_fs_file_data_t* file_data) {
    if (!file_data) {
        return;
    }
    free(file_data->data);
    free(file_data);
}

void lib_fs_file_data_list_free(lib_fs_file_data_t** file_list, int file_count) {
    if (!file_list) {
        return;
    }

    // TODO: Use size_t
    if (file_count <= 0) {
      free(file_list);
      return;
    }

    for (int i = 0; i < file_count; i++) {
        lib_fs_file_data_free(file_list[i]);
    }
    free(file_list);
}

int lib_io_read_cat_bytes(const char** file_names, size_t file_count, char** data, size_t* size) {

    if (data) {
        *data = NULL;
    }
    if (size) {
        *size = 0;
    }

    if (!file_names || !data || !size) {
        return -1;
    }

    if (file_count == 0) {
        return 0;
    }

    lib_fs_file_data_t** file_list = _lib_io_file_list_new(file_count);
    if (!file_list) {
        //*size = 0;
        // error: mem
        return -1;
    }

    lib_fs_file_data_t* file_data = NULL;
    const char* file_name = NULL;
    //size_t file_size = 0;
    size_t total_size = 0;

    for (int i = 0; i < file_count; i++) {

        file_name = file_names[i];
        if (!file_name) {
          file_list[i] = NULL;
          continue;
        }

        file_data = _lib_io_file_data_new();
        if (!file_data) {
            lib_fs_file_data_list_free(file_list, file_count);
            //*size = 0;
            // TODO: ups may be continue
            return -1;
        }

        char* input_data = NULL;
        int retval = lib_io_read_all_bytes(file_name, &input_data);

        if (retval < 0 || !input_data) {
            lib_fs_file_data_free(file_data);
            lib_fs_file_data_list_free(file_list, file_count);
            //*size = 0;
            // TODO: ups may be continue
            return -1;
        }

        file_data->data = input_data;
        file_data->size = retval;

        file_list[i] = file_data;
        total_size += file_data->size;
    }

    // V1
    char* total_data = _data_new(total_size);
    if (!total_data) {
        lib_fs_file_data_list_free(file_list, file_count);
        //*size = 0;
        return -1;
    }

    int offset = 0;

    for (int j = 0; j < file_count; j++) {

        file_data = file_list[j];
        if (!file_data) {
          continue;
        }
        
        for (int i = 0; i < file_data->size; i++) {
            total_data[i + offset] = file_data->data[i];
        }
        offset += file_data->size;

    }

    lib_fs_file_data_list_free(file_list, file_count);

    *data = total_data;
    *size = total_size;

    return 0;

}

// https://www.delftstack.com/howto/c/read-binary-file-in-c/
