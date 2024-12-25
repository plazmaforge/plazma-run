#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "config.h"
#include "iolib.h"

#define LIB_IO_FSIZE_STAT 1
#define LIB_IO_FSIZE_SEEK 2
#define LIB_IO_FSIZE_PURE 3 

#define LIB_IO_FSIZE_MODE LIB_IO_FSIZE_STAT

#define LIB_IO_USE_RBUF   false
#define LIB_IO_USE_WBUF   false
#define LIB_IO_USE_BUF    false

#define LIB_IO_RBUF_SIZE  4096
#define LIB_IO_WBUF_SIZE  4096
#define LIB_IO_BUF_SIZE   4096

typedef struct lib_io_mode_t {
  bool use_bin;    /* bin/txt                           */ 
  bool use_buf;    /* bufferred                         */
  bool use_fsize;  /* get file size (read only)         */
  bool use_msize;  /* alloc mem by all size (read only) */
  bool use_check;  /* full check read/write bytes       */
  bool read_all;   /* read all data (read only)         */
} lib_io_mode_t;

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

static long _file_size(const char* file_name, FILE* file) {
  long size = 0;
  if (LIB_IO_FSIZE_MODE == LIB_IO_FSIZE_STAT) {
    // stat
    size = _file_size_stat(file_name);
  } else if (LIB_IO_FSIZE_MODE == LIB_IO_FSIZE_SEEK) {
    // seek
    size = _file_size_fseek(file);
  } else {
    // default
    size = _file_size_stat(file_name);
  }

  if (size < 0) {
    #ifdef LIB_ERROR
    fprintf(stderr, "ERROR: _file_size: file_name=%s\n", file_name);
    #endif
    return size;
  }

  #ifdef LIB_DEBUG
  fprintf(stderr, "DEBUG: _file_size: file_name=%s, file_size=%lu\n", file_name, size);
  #endif

  return size;
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

static int _init_mode(lib_io_mode_t* mode) {
  if (!mode) {
    return -1;
  }
  mode->use_bin    = true;
  mode->use_buf    = LIB_IO_USE_BUF;
  mode->use_fsize  = true;
  mode->use_msize  = true;
  mode->use_check  = false;
  mode->read_all   = false;
  return 0;
}

static FILE* _fopen(const char* file_name, const char* mode) {
  FILE* file = fopen(file_name, mode);
  if (!file) {
    #ifdef LIB_ERROR
    fprintf(stderr, "ERROR: _fopen: file_name=%s\n", file_name);
    #endif     
    return file;
  }

  #ifdef LIB_DEBUG
  fprintf(stderr, "DEBUG: _fopen: file_name=%s\n", file_name);
  #endif

  return file;
}

static int	_fclose(FILE* file) {
  int retval = fclose(file);
  #ifdef LIB_DEBUG
  fprintf(stderr, "DEBUG: _fclose\n");
  #endif
  return retval;
}

static int _lib_io_read(lib_io_mode_t* mode, const char* file_name, char** data, size_t size, size_t* out_size) {

  #ifdef LIB_DEBUG
  fprintf(stderr, ">> io_read_bytes: size=%lu\n", size);
  #endif

  if (data) {
    *data = NULL;
  }

  if (out_size) {
    *out_size = 0;
  }

  if (size == 0 && !mode->read_all) {
    // no data
    #ifdef LIB_DEBUG
    fprintf(stderr, ">> io_read_bytes: return: size=%lu\n", size);
    #endif
    return 0;
  }

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

  if (!mode->use_buf) {
    // No buffer: Need file size
    mode->use_fsize = true;
    mode->use_msize = true;
  }

  // Read
  FILE* file = _fopen(file_name, "rb");
  if (!file) {
    // error: io
    return -1;
  }

  if (mode->use_fsize) {
    size_t file_size = 0;
    if ((file_size = _file_size(file_name, file)) < 0) {
      // error: io
      _fclose(file);
      return -1;
    }

    if (mode->read_all || size > file_size) {
      size = file_size;
    }
  }

  char* _data  = NULL;
  size_t _size = 0;

  if (!mode->use_buf) {

    // Full read
    _data = _data_new(size);
    if (!_data) {
      // error: mem
      _fclose(file);
      return -1;
    }

    _size = fread(_data, sizeof(char), size, file);

  } else {

    // Buffered read    
    // error: not implemented yet
    _fclose(file);
    return -2;
  }

  #ifdef LIB_DEBUG
  fprintf(stderr, ">> io_read_bytes: read: size=%lu\n", _size);
  #endif

  _data[_size] = '\0'; /* NUL-terminated */
  *data = _data;
  if (out_size) {
      *out_size = _size;
  }

  _fclose(file);
  return 0;

}

static int _lib_io_write(lib_io_mode_t* mode, const char* file_name, char* data, size_t size, size_t* out_size) {

  #ifdef LIB_DEBUG
  fprintf(stderr, ">> io_write_bytes: size=%lu\n", size);
  #endif

  if (out_size) {
    *out_size = 0;
  }

  if (size == 0) {
    // no data
    #ifdef LIB_DEBUG
    fprintf(stderr, ">> io_write_bytes: return: size=%lu\n", size);
    #endif
    return 0;
  }

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

  // Write
  FILE* file = _fopen(file_name, "wb+");
  if (!file) {
    // error: io
    return -1;
  }

  size_t _size = 0;

  if (!mode->use_buf) {
    _size = fwrite(data, sizeof(char), size, file);
  } else {
    _fclose(file);
    // error: not implemented yet
    return -2;
  }
 
  #ifdef LIB_DEBUG
  fprintf(stderr, ">> io_write_bytes: write: size=%lu\n", _size);
  #endif

  if (out_size) {
    *out_size = _size;
  }

  _fclose(file);
  return 0;
}

////

int lib_io_read_all_bytes(const char* file_name, char** data, size_t* size) {
  lib_io_mode_t mode;
  _init_mode(&mode);
  mode.read_all = true; // for load all data 
  return _lib_io_read(&mode, file_name, data, 0, size);
}

int lib_io_read_bytes(const char* file_name, char** data, size_t size, size_t* out_size) {
  lib_io_mode_t mode;
  _init_mode(&mode);
  return _lib_io_read(&mode, file_name, data, size, out_size);
}

/*
int lib_io_read_all_bytes(const char* file_name, char** data) {
  lib_io_mode_t mode;
  _init_mode(&mode);
  mode.read_all = true; // for load all data 
  return _lib_io_read(&mode, file_name, data, 0, NULL);
}

int lib_io_read_bytes(const char* file_name, char** data, size_t size) {
  lib_io_mode_t mode;
  _init_mode(&mode);
  return _lib_io_read(&mode, file_name, data, size, NULL);
}
*/

////

int lib_io_write_all_bytes(const char* file_name, char* data, size_t size, size_t* out_size) {
  lib_io_mode_t mode;
  _init_mode(&mode);
  return _lib_io_write(&mode, file_name, data, size, out_size);
}

int lib_io_write_bytes(const char* file_name, char* data, size_t size, size_t* out_size) {
  lib_io_mode_t mode;
  _init_mode(&mode);
  return _lib_io_write(&mode, file_name, data, size, out_size);
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
        // error: mem
        return -1;
    }

    lib_fs_file_data_t* file_data = NULL;
    const char* file_name = NULL;
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
            // error: mem
            return -1;
        }

        char* data       = NULL;
        size_t data_size = 0;

        int retval = lib_io_read_all_bytes(file_name, &data, &data_size);
        if (retval < 0 || !data) {
            free(data);
            data      = NULL;
            data_size = 0;

            lib_fs_file_data_free(file_data);
            lib_fs_file_data_list_free(file_list, file_count);
            // TODO: ups may be continue
            return -1;
        }

        file_data->data = data;
        file_data->size = data_size;

        file_list[i] = file_data;
        total_size += file_data->size;
    }

    // V1
    char* total_data = _data_new(total_size);
    if (!total_data) {
        lib_fs_file_data_list_free(file_list, file_count);
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
