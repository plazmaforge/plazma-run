#include <stdlib.h> /* malloc, free             */
#include <stdarg.h> /* va_start, va_arg, va_end */
#include <ctype.h>

#include <stdio.h>

#include "strlib.h"

#define LIB_STR_Q1  '\''
#define LIB_STR_Q2  '"'
#define LIB_STR_Q   '"'

#define LIB_STR_EMP ""
#define LIB_STR_BLK " "

/**
 * [allocate]
 */
static char* _lib_strndup(const char* src, size_t num) {
  char* dst = lib_strnew(num);
  if (!dst) {
    return NULL;
  }    
  memcpy(dst, src, num);
  //dst[num] = '\0';
  return dst;
}

static bool _lib_is_strnqta(const char* src, size_t num) {
  return lib_is_strnqtc(src, num, LIB_STR_Q1) || lib_is_strnqtc(src, num, LIB_STR_Q2);
}

/**
 * Returns lenght of string arguments 
 */
static size_t _lib_vstrlen(const char* first, va_list args) {  
  size_t len = 0;
  const char* arg;
  for (arg = first; arg != NULL; arg = va_arg(args, const char*)) {
    len += strlen(arg);
  }
  return len;
}

/**
 * Concatenates string arguments 
 */
static char* _lib_vstrcat(char* dst, const char* str, va_list args) {
  size_t len = 0;
  const char* arg;
  for (arg = str; arg != NULL; arg = va_arg(args, const char*)) {
    strcat(dst, arg);
  }
  return dst;
}

static char* _lib_vstrcpy(char *dst, const char *first, va_list args) {
  size_t len = 0;
  char *end = dst;
  const char *arg;
  for (arg = first; arg; arg = va_arg (args, const char*)) {
    len = strlen(arg);
    memcpy(end, arg, len);
    //strcpy(end, arg);
    end += len;
  }
  *end = '\0';
  return dst;
}

// strlen

size_t lib_strlen(const char* str) {
  if (!str) {
    return 0;
  }
  return strlen(str);
}

size_t lib_strnlen(const char* str, size_t num) {
  if (!str) {
    return 0;
  }
  return strnlen(str, num);
}

// strnew

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

// strdup

/**
 * [allocate]
 */
char* lib_strdup(const char* src) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  return _lib_strndup(src, len);
}

/**
 * [allocate]
 */
char* lib_strndup(const char* src, size_t num) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  len = len < num ? len : num;
  return _lib_strndup(src, len);
}

/**
 * [allocate]
 */
char* lib_strdupqt(const char* src) {
    return lib_strdupqtc(src, LIB_STR_Q); // default
}

/**
 * [allocate]
 * Quote char
 */
char* lib_strdupqtc(const char* src, char quote) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  char* dst = NULL;
  if (lib_is_strnqtc(src, len, quote)) {
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
char* lib_strdupuq(const char* src) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  if (_lib_is_strnqta(src, len)) {
     return lib_strndup(src + 1, len - 2);
  } else {
     return lib_strdup(src);
  }
  //return lib_strdupuqc(src, LIB_STR_Q);
}

/**
 * [allocate]
 * Quote char
 */
char* lib_strdupuqc(const char* src, char quote) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  if (lib_is_strnqtc(src, len, quote)) {  
     return lib_strndup(src + 1, len - 2);
  } else {
     return lib_strdup(src);
  }
}

// is_strqt/qtc

bool lib_is_strqt(const char* str) {
  if (!str) {
    return 0;
  }
  return lib_is_strnqt(str, strlen(str));
}

bool lib_is_strnqt(const char* str, size_t num) {
  if (!str || num == 0) {
    return false;
  }
  return _lib_is_strnqta(str, num);
}

bool lib_is_strqtc(const char* str, char quote) {
  if (!str) {
    return false;
  }
  return lib_is_strnqtc(str, strlen(str), quote);
}

bool lib_is_strnqtc(const char* str, size_t num, char quote) {
  if (!str || num < 2) { // start, end
    return false;
  }
  return str[0] == quote && str[num - 1] == quote;
}

// strcpy

char* lib_strcpy(char* dst, const char* src) {
  if (!dst || !src) {
    return dst;
  }
  return strcpy(dst, src);
}

char* lib_strncpy(char* dst, const char* src, size_t num) {
  if (!dst || !src) {
    return dst;
  }
  return strncpy(dst, src, num);
}

size_t lib_strlcpy(char* dst, const char* src, size_t num) {
  if (!dst || !src) {
    return 0;
  }
  return strlcpy(dst, src, num);
}

char* lib_strcpyv_(char* dst, const char* str, ...) {

  if (!dst || !str) {
    return dst;
  }

  va_list args;  
  va_start(args, str);
  _lib_vstrcpy(dst, str, args);
  va_end(args);

  return dst;
}

// strcat

char* lib_strcat(char* dst, const char* src) {
  if (!dst || !src) {
    return dst;
  }
  return strcat(dst, src);
}

char* lib_strncat(char* dst, const char* src, size_t num) {
  if (!dst || !src) {
    return dst;
  }
  return strncat(dst, src, num);
}

size_t lib_strlcat(char* dst, const char* src, size_t num) {
  if (!dst || !src) {
    return 0;
  }
  return strlcat(dst, src, num);
}

char* lib_strcatv_(char* dst, const char* str, ...) {

  if (!dst || !str) {
    return dst;
  }

  va_list args;  
  size_t len = 0;

  va_start(args, str);
  len = _lib_vstrlen(str, args);
  va_end(args);

  if (len == 0) {
    return dst;
  }

  va_start(args, str);
  _lib_vstrcat(dst, str, args);
  va_end(args);

  return dst;
}

// strjoin

char* lib_strjoin_(const char* str, ...) {
  if (!str) {
    return NULL;
  }

  va_list args;
  size_t len = 0;

  va_start(args, str);
  len = _lib_vstrlen(str, args);
  va_end(args);

  if (len == 0) {
    return lib_strdup(LIB_STR_EMP);
  }

  char* dst = lib_strnew(len);
  if (!dst) {
    return NULL;
  }

  va_start(args, str);
  _lib_vstrcat(dst, str, args);
  va_end(args);

  return dst;
}

char* lib_strcjoin(size_t count, const char* str, ...) {
  if (count == 0) {
    return NULL;
  }

  va_list args;
  const char* arg;
  size_t len = 0;
  size_t i = 0;
  int null_count = 0;

  va_start(args, str);
  arg = str; 
  for (i = 0; i < count; i++) {
    if (arg) {
      len += strlen(arg);
    } else {
      null_count++;
    }    
    arg = va_arg(args, const char*);
  }
  va_end(args);

  if (i == null_count) {
    return NULL;
  }

  if (len == 0) {
    return lib_strdup(LIB_STR_EMP);
  }

  char* dst = lib_strnew(len);
  if (!dst) {
    return NULL;
  }

  va_start(args, str);
  arg = str;
  for (i = 0; i < count; i++) {    
    if (arg) {
      lib_strcat(dst, arg);
    }    
    arg = va_arg(args, const char*);
  }
  va_end(args);

  return dst;
}

//

const char* lib_strchr(const char* str, int c) {
  if (!str) {
    return NULL;
  }
  return strchr(str, c);
}

const char* lib_strrchr(const char* str, int c) {
  if (!str) {
    return NULL;
  }
  return strrchr(str, c);
}

char* lib_strchr_end(char* str, int c) {
	if (!str) {
		return NULL;
	}
	char* val;
	for (val = str; *val && *val != c; ++val);
	return val;
}

size_t lib_stridx(const char* str, int c) {
  if (!str) {
    return -1;
  }
  const char* res = lib_strchr(str, c);
  return (res) ? res - str : -1;
}

// strstr

const char* lib_strstr(const char* str, const char* find) {
  if (!str || !find) {
    return NULL;
  }
  //return strstr(str, find);

  char* s = (char*) str;
  char* f;
  size_t flen = strlen(find);

  while (*s) {
    f = (char*) find;
    if (*s == *f) {
      size_t j = 1;
      while (*s == *f) {
        if (j == flen) {
          return (char*) (s - flen + 1);
        }
        s++;
        f++;
        j++;
      }      
    }
    s++;
  }
  return NULL;
}

const char* lib_strnstr(const char* str, const char* find, size_t num) {
  if (!str || !find) {
    return NULL;
  }
  //return strnstr(str, find, num);

  char* s = (char*) str;
  char* f;
  size_t flen = strlen(find);
  size_t i = 0;

  while (*s && i < num) {
    f = (char*) find;
    if (*s == *f) {
      size_t j = 1;
      while (*s == *f) {
        if (j == flen) {
          return (char*) (s - flen + 1);
        }
        s++;
        f++;
        j++;
      }      
    }
    i++;
    s++;
  }
  return NULL;  
}

//

bool lib_stremp(const char* str) {
    return !str || str[0] == '\0';
}

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

int lib_strncmp(const char* str1, const char* str2, size_t num) {
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
  return strncmp(str1, str2, num);
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

int lib_strnicmp(const char* str1, const char* str2, size_t num) {
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

  while (i < num) {
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

// strlwr/upr

char* lib_strlwr(char* str) {
    if (!str) {
        return NULL;
    }
    char* s = str;
    while (*s) {
      *s = tolower(*s);
      s++;
    }
    return str;
}

char* lib_strupr(char* str) {
    if (!str) {
        return NULL;
    }
    char* s = str;
    while (*s) {
      *s = toupper(*s);
      s++;
    }
    return str;
}

// strrev

char* lib_strrev(char* str) {
    if (!str) {
        return NULL;
    }
    size_t len = strlen(str);
    if (len < 2) {
      return str;
    }
    size_t k = len / 2;
    size_t i;
    size_t j;
    char c;
    for (i = 0, j = len - 1; i < k; i++, j--) {
      c = str[i];
      str[i] = str[j];
      str[j] = c;
    }
    return str;
}

char* lib_strsep(char** str, const char* delims) {
    if (!str || !delims) {
        return NULL;
    }
    return strsep(str, delims);
}

char* lib_strtok(char* str, const char* delims) {
    // str can be NULL
    if (!delims) {
      return NULL;
    }
    return strtok(str, delims);
}

// strtrc

char* lib_strtrc(char* str, char from, char to) {
    if (!str) {
        return NULL;
    }
    return lib_strntrc(str, lib_strlen(str), from, to);
}

char* lib_strntrc(char* str, size_t num, char from, char to) {
    if (!str) {
        return NULL;
    }
    for (size_t i = 0; i < num; i++) {
        if (str[i] == from) {
            str[i] = to;
        }
    }
    return str;
}

// strsaf

const char* lib_strsaf(const char* str) {
    return str ? str : LIB_STR_EMP;
}

// stralen

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

// strtok vs strsep
// https://stackoverflow.com/questions/7218625/what-are-the-differences-between-strtok-and-strsep-in-c