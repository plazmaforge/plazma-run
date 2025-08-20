#include <ctype.h>

#include "md5.h"
#include "sha1.h"
#include "sha256.h"
#include "sha512.h"

#include "run_mdlib.h"

static int _lib_sha224(const unsigned char* idata, size_t isize, unsigned char* odata) {
    return lib_sha256(idata, isize, odata, 1);
}

static int _lib_sha256(const unsigned char* idata, size_t isize, unsigned char* odata) {
    return lib_sha256(idata, isize, odata, 0);
}

static int _lib_sha384(const unsigned char* idata, size_t isize, unsigned char* odata) {
    return lib_sha512(idata, isize, odata, 1);
}

static int _lib_sha512(const unsigned char* idata, size_t isize, unsigned char* odata) {
    return lib_sha512(idata, isize, odata, 0);
}

////

static int _lib_stricmp(const char* str1, const char* str2) {
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

static int _lib_md_init(void* cnf, const char* name) {
    if (!cnf || !name) {
        return 1;
    }
    lib_md_config_t* config = (lib_md_config_t*) cnf;
    if (_lib_stricmp(name, "MD5") == 0) {
        config->md_name = LIB_MD_MD5_NAME; // "MD5"
        config->md_size = LIB_MD_MD5_SIZE; // 16
        config->md_func = lib_md5;
        return 0;
    } else if (_lib_stricmp(name, "SHA1") == 0) {
        config->md_name = LIB_MD_SHA1_NAME; // "SHA1"
        config->md_size = LIB_MD_SHA1_SIZE; // 20
        config->md_func = lib_sha1;
        return 0;
    } else if (_lib_stricmp(name, "SHA224") == 0) {
        config->md_name = LIB_MD_SHA224_NAME; // "SHA224"
        config->md_size = LIB_MD_SHA224_SIZE; // 28
        config->md_func = _lib_sha224;
        return 0;
    } else if (_lib_stricmp(name, "SHA256") == 0) {
        config->md_name = LIB_MD_SHA256_NAME; // "SHA256"
        config->md_size = LIB_MD_SHA256_SIZE; // 32
        config->md_func = _lib_sha256;
        return 0;
    } else if (_lib_stricmp(name, "SHA384") == 0) {
        config->md_name = LIB_MD_SHA384_NAME; // "SHA384"
        config->md_size = LIB_MD_SHA384_SIZE; // 48
        config->md_func = _lib_sha384;
        return 0;
    } else if (_lib_stricmp(name, "SHA512") == 0) {
        config->md_name = LIB_MD_SHA512_NAME; // "SHA512"
        config->md_size = LIB_MD_SHA512_SIZE; // 64
        config->md_func = _lib_sha512;
        return 0;
    }
    return 1;
}

int main(int argc, char* argv[]) {

    lib_md_config_t config;
    config.md_name = NULL;
    config.md_size = 0;
    config.md_func = NULL;
    config.md_init = _lib_md_init;

    return run_md(&config, argc, argv);
}