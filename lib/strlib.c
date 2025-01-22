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

size_t lib_strlcpy(char* dst, const char* src, size_t dsize) {
  if (!dst || !src || dsize == 0) {
    return 0;
  }
  //return strlcpy(dst, src, size);

  const char *osrc = src;
	size_t n = dsize;

	/* Copy as many bytes as will fit. */
	while (--n != 0) {
		if ((*dst++ = *src++) == '\0') {
      break;
    }
	}

	//while (--n != 0 && (*dst++ = *src++) != '\0');

	/* Not enough room in dst, add NUL and traverse rest of src. */
	if (n == 0) {
		if (dsize != 0) {
      *dst = '\0';		/* NUL-terminate dst */
    }
		while (*src++);
	}

	return(src - osrc - 1);	/* count does not include NUL */  
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

size_t lib_strlcat(char* dst, const char* src, size_t dsize) {
  if (!dst || !src || dsize == 0) {
    return 0;
  }
  //return strlcat(dst, src, dsize);

  const char *odst = dst;
	const char *osrc = src;
	size_t n = dsize;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end. */
	while (n-- != 0 && *dst != '\0') {
    dst++;
  }
		
	dlen = dst - odst;
	n = dsize - dlen;

	if (n-- == 0) {
    return (dlen + strlen(src));
  }

	while (*src != '\0') {
		if (n != 0) {
			*dst++ = *src;
			n--;
		}
		src++;
	}
	*dst = '\0';

	return (dlen + (src - osrc));	/* count does not include NUL */
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

/**
 * Find one string inside another, ignoring case
 */
const char* lib_stristr(const char* str, const char* find) {
  if (!str || !find) {
    return NULL;
  }

  char* s = (char*) str;
  char* f;
  size_t flen = strlen(find);

  while (*s) {
    f = (char*) find;
    if (tolower(*s) == tolower(*f)) {
      size_t j = 1;
      // TODO: Use do/while
      while (tolower(*s) == tolower(*f)) {
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

/**
 * Find one string inside another, ignoring case, up to a maximum length
 */
const char* lib_strnistr(const char* str, const char* find, size_t num) {
  if (!str || !find) {
    return NULL;
  }

  char* s = (char*) str;
  char* f;
  size_t flen = strlen(find);
  size_t i = 0;

  while (*s && i < num) {
    f = (char*) find;
    if (tolower(*s) == tolower(*f)) {
      size_t j = 1;
      while (tolower(*s) == tolower(*f)) {
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

char* lib_strsep(char** str, const char* delim) {
    if (!str || !*str || !delim) {
        return NULL;
    }
    //return strsep(str, delim);

    char *s = *str;
    const char *spanp;
    int c;
    int sc;
    char *tok;

    //if ((s = *str) == NULL) {
    //  return NULL;
    //}

    for (tok = s;;) {
      c = *s++;
      spanp = delim;
      do {
        if ((sc = *spanp++) == c) {
          if (c == 0)
            s = NULL;
          else
            s[-1] = 0;
          *str = s;
          return tok;
        }
      } while (sc != 0);
    }

    return NULL;
}

char* lib_strtok(char* str, const char* delim) {
    // str can be NULL
    if (!delim) {
      return NULL;
    }
    return strtok(str, delim);
}

// strtrc

static char* _lib_strntrc(char* str, size_t num, char from, char to) {
    // if (!str) {
    //     return NULL;
    // }
    // if (num == 0) {
    //   return str;
    // }
    if (from == to) {
      return str;
    }
    for (size_t i = 0; i < num; i++) {
        if (str[i] == from) {
            str[i] = to;
        }
    }
    return str;
}

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
    if (num == 0) {
      return str;
    }
    return _lib_strntrc(str, num, from, to);
}

//

char* lib_strtrs(char* str, const char* from, const char* to) {
    if (!str) {
        return NULL;
    }
    return lib_strntrs(str, lib_strlen(str), from, to);
}

char* lib_strntrs(char* str, size_t num, const char* from, const char* to) {
    if (!str) {
        return NULL;
    }
    if (num == 0) {
      return str;
    }
    size_t len = strlen(from);
    size_t len2 = strlen(to);
    if (len2 < len) {
      len = len2;
    }
    if (len == 0) {
      return str;
    }

    //bool diff = false;
    //for (size_t i = 0; i < len; i++) {
    //  if (from[i] != to[i]) {
    //    diff = true;
    //  }
    //}
    //if (!diff) {
    //  return str;
    //}

    for (size_t i = 0; i < len; i++) {
      _lib_strntrc(str, num, from[i], to[i]);
    }
    return str;
}

// strsafe

const char* lib_strsafe(const char* str) {
    return str ? str : LIB_STR_EMP;
}

// stralen

char** lib_strarrnew(size_t size) {
    char** arr = (char**) malloc(size + 1);
    if (!arr) {
      return NULL;
    }
    arr[size] = NULL;
    return arr;
}

size_t lib_strarrlen(/*const*/ char** arr) {
    if (!arr) {
        return 0;
    }
    /*const*/ char* e = NULL;
    /*const*/ char** a = arr;
    size_t len = 0;
    while ((e = *a) != NULL) {
        len++;
        a++;
    }
    return len;
}

void lib_strarrfree(char** arr) {
    if (!arr) {
        return;
    }
    char* e = NULL;
    char** a = arr;
    while ((e = *a) != NULL) {
        free(e);
        a++;
    }
    free(arr);
}

//

bool lib_strstarts(const char* str, const char* val) {
  if (!str || !val) {
    return false;
  }
  size_t total = strlen(str);
  if (total == 0) {
    return false;
  }
  size_t len = strlen(val);
  if (len == 0 || len > total) {
    return false;
  }
  char* s = (char*) str;
  char* v = (char*) val;
  for (size_t i = 0; i < len; i++) {
    if (*s != *v) {
      return false;
    }
    s++;
    v++;
  }
  return true;
}

bool lib_strends(const char* str, const char* val) {
  if (!str || !val) {
    return false;
  }
  size_t total = strlen(str);
  if (total == 0) {
    return false;
  }
  size_t len = strlen(val);
  if (len == 0 || len > total) {
    return false;
  }
  char* s = (char*) (str + (total - len));
  char* v = (char*) val;
  for (size_t i = 0; i < len; i++) {
    if (*s != *v) {
      return false;
    }
    s++;
    v++;
  }
  return true;
}

//

bool lib_stristarts(const char* str, const char* val) {
  if (!str || !val) {
    return false;
  }
  size_t total = strlen(str);
  if (total == 0) {
    return false;
  }
  size_t len = strlen(val);
  if (len == 0 || len > total) {
    return false;
  }
  char* s = (char*) str;
  char* v = (char*) val;
  for (size_t i = 0; i < len; i++) {
    if (tolower(*s) != tolower(*v)) {
      return false;
    }
    s++;
    v++;
  }
  return true;
}

bool lib_striends(const char* str, const char* val) {
  if (!str || !val) {
    return false;
  }
  size_t total = strlen(str);
  if (total == 0) {
    return false;
  }
  size_t len = strlen(val);
  if (len == 0 || len > total) {
    return false;
  }
  char* s = (char*) (str + (total - len));
  char* v = (char*) val;
  for (size_t i = 0; i < len; i++) {
    if (tolower(*s) != tolower(*v)) {
      return false;
    }
    s++;
    v++;
  }
  return true;
}

/**
 * 
 * [alocate]
 */
char* lib_strleft(const char* str, size_t len) {
  if (!str || len == 0) {
    return NULL;
  }
  size_t total = strlen(str);
  if (total == 0) {
    return NULL; // empty ?
  }
  if (len >= total) {
    return lib_strdup(str);
  }  
  return lib_strndup(str, len);
}

/**
 * 
 * [alocate]
 */
char* lib_strright(const char* str, size_t len) {
  if (!str || len == 0) {
    return NULL;
  }
  size_t total = strlen(str);
  if (total == 0) {
    return NULL; // empty ?
  }
  if (len >= total) {
    return lib_strdup(str);
  }  
  return lib_strndup(str + (total - len), len);
}

/**
 * Return a substring of a string
 * 
 * [alocate]
 */
char* lib_strsub(const char* str, size_t start, size_t len) {
  if (!str) {
    return NULL;
  }
  size_t total = strlen(str);
  if (total == 0 
    || start > total - 1 
    || start + len > total - 1) {
    return NULL;  // empty ?
  }
  return lib_strndup(str + start, len);
}

////

static char* _lib_strcpylwr(char* dst, const char* str) {
  char* s = (char*) str;
  char* d = dst;
  while (*s) {
      *d = tolower(*s);
      s++;
      d++;
  }
  return dst;
}

char* lib_strcpylwr(char* dst, const char* str) {
  if (!dst) {
    return NULL;
  }
  if (!str) {
    return dst;
  }
  return _lib_strcpylwr(dst, str);
}

/**
 * Create new string and convert a string to lowercase.
 * 
 * [alocate]
 */
char* lib_tostrlwr(const char* str) {
  if (!str) {
    return NULL;
  }
  
  size_t len = strlen(str);  
  char* dst = lib_strnew(len);
  if (!dst) {
    return NULL;
  }
  if (len == 0) {
    return dst;
  }
  return _lib_strcpylwr(dst, str);
}

static char* _lib_strcpyupr(char* dst, const char* str) {
  char* s = (char*) str;
  char* d = dst;
  while (*s) {
      *d = toupper(*s);
      s++;
      d++;
  }
  return dst;
}

char* lib_strcpyupr(char* dst, const char* str) {
  if (!dst) {
    return NULL;
  }
  if (!str) {
    return dst;
  }
  return _lib_strcpyupr(dst, str);
}

/**
 * Create new string and convert a string to uppercase.
 * 
 * [alocate]
 */
char* lib_tostrupr(const char* str) {
  if (!str) {
    return NULL;
  }
  
  size_t len = strlen(str);  
  char* dst = lib_strnew(len);
  if (!dst) {
    return NULL;
  }
  if (len == 0) {
    return dst;
  }
  return _lib_strcpyupr(dst, str);
}


char* lib_strcpycase(char* dst, const char* str, int mode) {
  if (!dst) {
    return NULL;
  }
  if (!str) {
    return dst;
  }
  
  if (mode == 1) {
    return lib_strcpylwr(dst, str);
  }
  if (mode == 2) {
    return lib_strcpyupr(dst, str);
  }
  //if (mode == 3) {
  //  return lib_strcpytit(dst, str);
  //}

  // No case - duplicate only
  return lib_strcpy(dst, str);
}

/**
 * Create new string and convert a string to case (lower/upper/title) by mode.
 * 
 * [alocate]
 */
char* lib_tostrcase(const char* str, int mode) {
  if (!str) {
    return NULL;
  }
  if (mode == 1) {
    return lib_tostrlwr(str);
  }
  if (mode == 2) {
    return lib_tostrupr(str);
  }
  //if (mode == 3) {
  //  return lib_tostrtit(str);
  //}

  // No case - duplicate only
  return lib_strdup(str);
}

/**
 * Calculate count of chars for this case mode 
 */
size_t lib_strcasecount(const char* str, int mode) {
  if (!str) {
    return 0;
  }
  if (mode == 1 || mode == 2 || mode == 3) {
    return strlen(str);
  }

  // TODO: Not implemnted yet
  return strlen(str);

  /*
  strcase.camelCase(string)      // "fooBar"
  strcase.snakeCase(string)      // "foo_bar"
  strcase.constantCase(string)   // "FOO_BAR"
  strcase.classCase(string)      // "FooBar"
  strcase.namespaceCase(string)  // "Foo.Bar"
  strcase.titleCase(string)      // "Foo Bar"
  strcase.paramCase(string)      // "foo-bar"
  strcase.pathCase(string)       // "foo/bar"
  strcase.dotCase(string)        // "foo.bar"
  */

}

// https://github.com/tower-archive/strcase

// https://www.unix.com/man-page/posix/7posix/string.h
// https://github.com/openbsd/src/tree/master/lib/libc/string
// https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/stpncpy.html
// https://www.rowleydownload.co.uk/arm/documentation/index.htm?https://www.rowleydownload.co.uk/arm/documentation/strncasestr.htm

// strtok vs strsep
// https://stackoverflow.com/questions/7218625/what-are-the-differences-between-strtok-and-strsep-in-c

// https://habr.com/ru/articles/838116/
// https://github.com/antirez/sds