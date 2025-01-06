#include <stdlib.h> /* malloc, free             */
#include <stdarg.h> /* va_start, va_arg, va_end */
#include <ctype.h>

#include "strlib.h"

#define LIB_STR_Q1  '\''
#define LIB_STR_Q2  '"'
#define LIB_STR_Q   '"'

#define LIB_STR_EMP ""
#define LIB_STR_BLK " "

static bool _lib_is_strn_qta(const char* src, size_t size) {
  return lib_is_strn_qtc(src, size, LIB_STR_Q1) || lib_is_strn_qtc(src, size, LIB_STR_Q2);
}

/**
 * [allocate]
 */
static char* _lib_strndup(const char* src, size_t size) {
  char* dst = lib_strnew(size);
  if (!dst) {
    return NULL;
  }    
  memcpy(dst, src, size);
  //dst[size] = '\0';
  return dst;
}

////

char* lib_strcat(char* dst, const char* src) {
  if (!dst || !src) {
    return dst;
  }
  return strcat(dst, src);
}

char* lib_strcpy(char* dst, const char* src) {
  if (!dst || !src) {
    return dst;
  }
  return strcpy(dst, src);
}

//

/**
 * [allocate]
 */
char* lib_strdup(const char* src) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  return _lib_strndup(src, len);

  // char* dst = lib_strnew(len);
  // if (!dst) {
  //   return NULL;
  // }    
  // memcpy(dst, src, len);
  // //dst[len] = '\0';
  // return dst;
}

/**
 * [allocate]
 */
char* lib_strndup(const char* src, size_t size) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  len = len < size ? len : size;
  return _lib_strndup(src, len);

  // char* dst = lib_strnew(len);
  // if (!dst) {
  //   return NULL;
  // }    
  // memcpy(dst, src, len);
  // //dst[len] = '\0';
  // return dst;
}

/**
 * [allocate]
 */
char* lib_strdup_qt(const char* src) {
    return lib_strdup_qtc(src, LIB_STR_Q); // default
}

/**
 * [allocate]
 * Quote char
 */
char* lib_strdup_qtc(const char* src, char quote) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  char* dst = NULL;
  if (lib_is_strn_qtc(src, len, quote)) {
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
char* lib_strdup_uq(const char* src) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  if (_lib_is_strn_qta(src, len)) {
     return lib_strndup(src + 1, len - 2);
  } else {
     return lib_strdup(src);
  }
  //return lib_strdup_uqc(src, LIB_STR_Q);
}

/**
 * [allocate]
 * Quote char
 */
char* lib_strdup_uqc(const char* src, char quote) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  if (lib_is_strn_qtc(src, len, quote)) {  
     return lib_strndup(src + 1, len - 2);
  } else {
     return lib_strdup(src);
  }
}

//

int lib_strcmp(const char* str1, const char* str2) {
  if (!str1 && !str2) {
    return 0;
  }
  if (!str1) {
    return -1;
  }
  if (!str2) {
    return 1;
  }
  if (str1 == str2) {
    return 0;
  }
  return strcmp(str1, str2);
}

int lib_strncmp(const char* str1, const char* str2, size_t size) {
  if (!str1 && !str2) {
    return 0;
  }
  if (!str1) {
    return -1;
  }
  if (!str2) {
    return 1;
  }
  if (str1 == str2) {
    return 0;
  }
  return strncmp(str1, str2, size);
}

int lib_stricmp(const char* str1, const char* str2) {
  if (!str1 && !str2) {
    return 0;
  }
  if (!str1) {
    return -1;
  }
  if (!str2) {
    return 1;
  }
  if (str1 == str2) {
    return 0;
  }

  /*
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  if (len1 < len2) {
    return -1;
  }
  if (len2 < len1) {
    return 1;
  }
  */

  /*
  for (size_t i = 0; i < len1; i++) {
    c1 = tolower(str1[i]);
    c2 = tolower(str2[i]);
    if (c1 < c2) {
      return -1;
    }
    if (c2 < c1) {
      return 1;
    }
  }
  */

  char* s1 = (char*) str1;
  char* s2 = (char*) str2;
  int c1   = 0;
  int c2   = 0;

  while (true) {
    if (*s1 == '\0') {
      return (*s2 == '\0') ? 0 : -1;
    }
    if (*s2 == '\0') {
      return (*s1 == '\0') ? 0 : 1;
    }
    c1 = tolower(*s1);
    c2 = tolower(*s2);
    if (c1 < c2) {
      return -1;
    }
    if (c2 < c1) {
      return 1;
    }
    s1++;
    s2++;
  }
  return 0;
}

int lib_strnicmp(const char* str1, const char* str2, size_t size) {
  if (!str1 && !str2) {
    return 0;
  }
  if (!str1) {
    return -1;
  }
  if (!str2) {
    return 1;
  }
  if (str1 == str2) {
    return 0;
  }
 
  char* s1 = (char*) str1;
  char* s2 = (char*) str2;
  int c1   = 0;
  int c2   = 0;
  size_t i = 0;

  while (i < size) {
    if (*s1 == '\0') {
      return (*s2 == '\0') ? 0 : -1;
    }
    if (*s2 == '\0') {
      return (*s1 == '\0') ? 0 : 1;
    }
    c1 = tolower(*s1);
    c2 = tolower(*s2);
    if (c1 < c2) {
      return -1;
    }
    if (c2 < c1) {
      return 1;
    }
    s1++;
    s2++;
    i++;    
  }
  return 0;
}

bool lib_stremp(const char* str) {
    return !str || str[0] == '\0';
}

const char* lib_strsaf(const char* str) {
    return str ? str : "";
}

/**
 * [allocate]
 */
char* lib_strnew(size_t size) {
  size_t _size = size * sizeof(char) + 1;
  char* str = (char*) malloc(_size);
  if (!str) {
    return NULL;
  }
  memset(str, 0, _size);
  return str;
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

// /**
//  * [allocate]
//  */
// char* lib_straddv_(const char* str1, const char* str2, const char* str3) {
//   if (!str1) {
//     return NULL;
//   }
//   char* dst = lib_strnew(lib_strlen(str1) + lib_strlen(str2) + lib_strlen(str3));
//   if (!dst) {
//     return NULL;
//   }
//   lib_strcpy(dst, str1);
//   if (str2) {
//     lib_strcat(dst, str2);
//   }
//   if (str3) {
//     lib_strcat(dst, str3);
//   }
//   return dst;
// }

char* lib_strapp(const char* str, ...) {

  va_list args;
  va_start(args, str);
  const char* s;
  size_t len = 0;
  int i = 0;

  for (s = str; s != NULL; s = va_arg(args, const char*)) {
    i++;
    len += strlen(s);
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
  for (s = str; s != NULL; s = va_arg(args, const char*)) {  
    lib_strcat(tmp, s);
  }
  va_end(args);

  return tmp;
}

char* lib_strappn(int n, const char* str, ...) {
  if (n <= 0) {
    return NULL;
  }

  va_list args;
  const char* s;
  size_t len = 0;
  size_t i = 0;
  int null_count = 0;

  va_start(args, str);
  s = str; 
  for (i = 0; i < n; i++) {
    if (s) {
      len += strlen(s);      
    } else {
      null_count++;
    }    
    s = va_arg(args, const char*);
  }
  va_end(args);

  if (i == null_count) {
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
  s = str;
  for (i = 0; i < n; i++) {    
    if (s) {
      lib_strcat(tmp, s);
    }    
    s = va_arg(args, const char*);
  }
  va_end(args);

  return tmp;
}

////

char* __lib_straddv__(const char* str, ...) {

  va_list args;
  va_start(args, str);
  char* arg;
  size_t len = 0;
  int i = 0;
  while((arg = va_arg(args, char *)) != NULL) {
    i++;
    len += strlen(arg);
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

char* __lib_straddn__(int n, const char* str, ...) {
  if (n <= 0) {
    return NULL;
  }

  va_list args;
  va_start(args, str);
  char* arg;
  size_t len = 0;
  size_t i = 0;
  int null_count = 0;
  for (i = 0; i < n; i++) {
    arg = va_arg(args, char*);
    if (!arg) {
      null_count++;
      continue;
    }
    len += strlen(arg);
  }
  va_end(args);

  if (i == null_count) {
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
  for (i = 0; i < n; i++) {
    arg = va_arg(args, char*);
    if (!arg) {
      continue;
    }
    lib_strcat(tmp, arg);
  }
  va_end(args);

  return tmp;
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

//

bool lib_is_str_qt(const char* src) {
  if (!src) {
    return 0;
  }
  return lib_is_strn_qt(src, strlen(src));
}

bool lib_is_strn_qt(const char* src, size_t size) {
  if (!src || size == 0) {
    return 0;
  }
  return _lib_is_strn_qta(src, size);
  //return lib_is_strn_qtc(src, size, LIB_STR_Q1) || lib_is_strn_qtc(src, size, LIB_STR_Q2);
}

bool lib_is_str_qtc(const char* src, char quote) {
  if (!src) {
    return 0;
  }
  return lib_is_strn_qtc(src, strlen(src), quote);
}

bool lib_is_strn_qtc(const char* src, size_t size, char quote) {
  if (!src || size < 2) { // start, end
    return false;
  }
  return src[0] == quote && src[size - 1] == quote;
}

//

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

void lib_strtr(char* str, char from, char to) {
    if (!str) {
        return;
    }
    lib_strntr(str, lib_strlen(str), from, to);
}

void lib_strntr(char* str, size_t len, char from, char to) {
    if (!str) {
        return;
    }
    for (size_t i = 0; i < len; i++) {
        if (str[i] == from) {
            str[i] = to;
        }
    }
}

char* lib_strchr_end(char* str, int c) {
	if (!str) {
		return NULL;
	}
	char* val;
	for (val = str; *val && *val != c; ++val);
	return val;
}
