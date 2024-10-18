#include <stdlib.h>
//#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "strlib.h"

int lib_stremp(const char* str) {
    return !str || str[0] == '\0';
}

const char* lib_strsaf(const char* str) {
    return str ? str : "";
}

/**
 * [allocate]
 */
char* lib_strnew(size_t size) {
  //if (size < 0) {
  //  return NULL;
  //}
  char* dst = (char*) malloc(size * sizeof(char) + 1);
  if (!dst) {
    return NULL;
  }
  for (int i = 0; i <= size; i++) {
    dst[i] = '\0';
  }    
  return dst;
}

/**
 * [allocate]
 */
char* lib_stradd(const char* str1, const char* str2) {
  if (!str1) {
    return NULL;
  }
  char* dst = lib_strnew(lib_strlen(str1) + lib_strlen(str2));
  if (!dst) {
    return NULL;
  }
  lib_strcpy(dst, str1);
  if (str2) {
    lib_strcat(dst, str2);
  }
  return dst;
}

/**
 * [allocate]
 */
char* lib_straddv_(const char* str1, const char* str2, const char* str3) {
  if (!str1) {
    return NULL;
  }
  char* dst = lib_strnew(lib_strlen(str1) + lib_strlen(str2) + lib_strlen(str3));
  if (!dst) {
    return NULL;
  }
  lib_strcpy(dst, str1);
  if (str2) {
    lib_strcat(dst, str2);
  }
  if (str3) {
    lib_strcat(dst, str3);
  }
  return dst;
}

char* __lib_straddv__(const char* str, ...) {

  va_list args;
  va_start(args, str);
  char* arg;
  size_t len = 0;
  int i = 0;
  while((arg = va_arg(args, char *)) != NULL) {
    i++;
    len += strlen(arg);
    //printf("%i: %s\n", i, arg);
  }
  va_end(args);

  if (i == 0) {
    return NULL;
  }

  if (len == 0) {
    return lib_strdup("");
  }

  char* tmp = lib_strnew(len);
  if (!tmp) {
    return NULL;
  }

  va_start(args, str);
  while((arg = va_arg(args, char *)) != NULL) {
    lib_strcat(tmp, arg);
  }
  va_end(args);

  return tmp;
}

char* lib_strcat(char* str1, const char* str2) {
  if (!str1) {
    return NULL;
  }
  if (!str2) {
    return str1;
  }
  return strcat(str1, str2);
}

char* lib_strcpy(char* dst, const char* src) {
  if (!dst) {
    return NULL;
  }
  if (!src) {
    return dst;
  }
  return strcpy(dst, src);
}

const char* lib_strchr(const char* str, int c) {
  if (!str) {
    return NULL;
  }
  return strchr(str, c);
}

size_t lib_strind(const char* str, int c) {
  if (!str) {
    return -1;
  }
  const char* res = lib_strchr(str, c);
  return (res) ? res - str : -1;
}

size_t lib_strlen(const char* src) {
  if (!src) {
    return 0;
  }
  return strlen(src);
}

/**
 * [allocate]
 */
char* lib_strdup(const char* src) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  char* dst = lib_strnew(len);
  if (!dst) {
    return NULL;
  }    
  memcpy(dst, src, len);
  //dst[len] = '\0';
  return dst;
}

/**
 * [allocate]
 */
char* lib_strndup(const char* src, size_t size) {
  if (!src || size < 0) {
    return NULL;
  }
  size_t len = strlen(src);
  len = len < size ? len : size;
  char* dst = lib_strnew(len);
  if (!dst) {
    return NULL;
  }    
  memcpy(dst, src, len);
  //dst[len] = '\0';
  return dst;
}

/**
 * [allocate]
 */
char* lib_strdup_qt_opts(const char* src, char quote) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  char* dst = NULL;
  if (!lib_is_strn_qt(src, len)) {
    dst = lib_strdup(src);
  } else {
    dst = lib_strnew(len + 2); // +2 start/end quotes
    if (!dst) {
      return NULL;
    }
    dst[0] = quote;       // start
    memcpy(++dst, src, len);
    len += 2;
    dst[len - 1] = quote; // end
    //dst[len] = '\0';
  }
  return dst;
}

/**
 * [allocate]
 */
char* lib_strdup_qt(const char* src) {
    return lib_strdup_qt_opts(src, '\'');
}

/**
 * [allocate]
 */
char* lib_strdup_uq(const char* src) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  if (lib_is_strn_qt(src, len)) {  
     return lib_strndup(src + 1, len - 2);
  } else {
     return lib_strdup(src);
  }
}

int lib_is_strn_qt_opts(const char* src, size_t size, char quote) {
  if (!src || size <= 0) {
    return 0;
  }
  return src[0] == quote && src[size - 1] == quote;
}

int lib_is_str_qt(const char* src, char quote) {
  if (!src) {
    return 0;
  }
  return lib_is_strn_qt_opts(src, strlen(src), quote);
}

int lib_is_strn_qt(const char* src, size_t size) {
  if (!src || size <= 0) {
    return 0;
  }
  return lib_is_strn_qt_opts(src, size, '\'') || lib_is_strn_qt_opts(src, size, '"');
}

int lib_is_str_qt(const char* src) {
  if (!src) {
    return 0;
  }
  return lib_is_strn_qt(src, strlen(src));
}

int lib_stralen(/*const*/ char** array) {
    if (!array) {
        return 0;
    }
    /*const*/ char* e = NULL;
    /*const*/ char** elements = array;
    int count = 0;
    while ((e = *elements) != NULL) {
        count++;
        elements++;
    }
    return count;
}

void lib_strafree(char** array) {
    if (!array) {
        return;
    }
    char* e = NULL;
    char** elements = array;
    int count = 0;
    while ((e = *elements) != NULL) {
        free(e);
        elements++;
    }
    free(array);
}

void lib_replace_n(char* path, size_t len, char from, char to) {
    if (!path) {
        return;
    }
    for (size_t i = 0; i < len; i++) {
        if (path[i] == from) {
            path[i] = to;
        }
    }
}

void lib_replace(char* path, char from, char to) {
    if (!path) {
        return;
    }
    lib_replace_n(path, lib_strlen(path), from, to);
}

char* lib_strchr_end(char* str, int c) {
	if (!str) {
		return NULL;
	}
	char* val;
	for (val = str; *val && *val != c; ++val);
	return val;
}
