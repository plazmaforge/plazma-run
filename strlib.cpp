#include <stdlib.h>
#include <string.h>

#include "strlib.h"

char* lib_strnew(size_t size) {
  if (size < 0) {
    return NULL;
  }
  char* dst = (char*) malloc(size + 1);
  if (!dst) {
    return NULL;
  }
  for (int i = 0; i <= size; i++) {
    dst[i] = '\0';
  }    
  //dst[size] = '\0';
  return dst;
}

char* lib_strnew(const char* str) {
  return lib_strdup(str);
}

char* lib_strnew(const char* str1, const char* str2) {
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

char* lib_strnew(const char* str1, const char* str2, const char* str3) {
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

char* lib_strnew(const char* str1, const char* str2, const char* str3, const char* str4) {
  if (!str1) {
    return NULL;
  }
  char* dst = lib_strnew(lib_strlen(str1) + lib_strlen(str2) + lib_strlen(str3) + lib_strlen(str4));
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
  if (str4) {
    lib_strcat(dst, str4);
  }
  return dst;
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

int lib_is_strn_qt(const char* src, size_t size, char quote) {
  if (!src || size <= 0) {
    return 0;
  }
  return src[0] == quote && src[size - 1] == quote;
}

int lib_is_str_qt(const char* src, char quote) {
  if (!src) {
    return 0;
  }
  return lib_is_strn_qt(src, strlen(src), quote);
}

int lib_is_strn_qt(const char* src, size_t size) {
  if (!src || size <= 0) {
    return 0;
  }
  return lib_is_strn_qt(src, size, '\'') || lib_is_strn_qt(src, size, '"');
}

int lib_is_str_qt(const char* src) {
  if (!src) {
    return 0;
  }
  return lib_is_strn_qt(src, strlen(src));
}

char* lib_strdup_qt(const char* src, char quote) {
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

char* lib_strdup_qt(const char* src) {
    return lib_strdup_qt(src, '\'');
}

char* lib_strdup_uq(const char* src) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  //char* dst = NULL;
  if (lib_is_strn_qt(src, len)) {  
     return lib_strndup(src + 1, len - 2);
  } else {
     return lib_strdup(src);
  }
  //return dst;
}
