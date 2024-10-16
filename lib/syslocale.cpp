#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>

#include "syslocale.h"
#include "strlib.h"

char* lib_locale_set_default_locale() {
  return setlocale(LC_ALL, "");
}

char* lib_locale_get_locale(int cat) {
  return setlocale(cat, NULL);
}

char* lib_locale_get_current_locale() {
  return lib_locale_get_locale(LC_CTYPE);
}

int lib_locale_is_empty(const char* locale) {
  return locale == NULL || (strcmp(locale, "C") == 0);
}

int lib_locale_equals(const char* locale1, const char* locale2) {
  if (locale1 == NULL && locale2 == NULL) {
    return 1;
  }
  if (locale1 == NULL || locale2 == NULL) {
    return 0;
  }
  return strcmp(locale1, locale2) == 0;
}

/*static*/ lib_locale_t* lib_locale_new() {
   lib_locale_t* locale = (lib_locale_t*) malloc(sizeof(lib_locale_t));
   locale->name = NULL;
   locale->language = NULL;
   locale->script = NULL;
   locale->country = NULL;
   locale->variant = NULL;
   locale->encoding = NULL;
   return locale;
}

/*static*/ void lib_locale_free(lib_locale_t* locale) {
    if (!locale) {
        return;
    }
    
    free(locale->name);
    free(locale->language);
    free(locale->script);
    free(locale->country);
    free(locale->variant);
    free(locale->encoding);
    
    free(locale);
}

char* lib_locale_parse_encoding(const char* locale) {
  if (!locale) {
    return NULL;
  }
  lib_locale_t* locale_s = lib_locale_parse_locale(locale);
  if (!locale_s) {
    return NULL;
  }
  char* encoding = NULL;
  if (locale_s->encoding) {
    encoding = strdup(locale_s->encoding);
  }
  lib_locale_free(locale_s);
  return encoding;
}

lib_locale_t* lib_locale_parse_locale(const char* locale) {
  if (!locale) {
    return NULL;
  }

  if (strcmp(locale, "C") == 0) {
    lib_locale_t* locale_s = lib_locale_new();
    locale_s->name = strdup(locale);
    return locale_s;
  }

  char* p = NULL;
  char* temp = strdup(locale);
  
  char* language = NULL;
  char* country = NULL;
  //char* encoding_variant = NULL;
  char* encoding = NULL;

  if ((encoding = strchr(temp, '.')) != NULL) {
    *encoding++ = '\0';
    //encoding = strdup(encoding); // TODO ???
  } else if ((encoding = strchr(temp, '@')) != NULL) {
    *encoding++ = '\0';
    //encoding = strdup(encoding); // TODO ???
  }

  if ((country = strchr(temp, '_')) != NULL) {
    *country++ = '\0';
    //country = strdup(country); // TODO ???
    language = temp;
  }

  if (language == NULL && country == NULL && encoding == NULL) {
    int len = strlen(temp);
    if (len == 2) {
      if (islower(temp[0]) && islower(temp[1])) {
        language = temp;
      } else {
        country = temp;
      }      
    } else {
      encoding = temp;
    }
    
  }
  
  lib_locale_t* locale_s = lib_locale_new();
  locale_s->name = strdup(locale);
  locale_s->language = language;
  locale_s->script = NULL;
  locale_s->country = country;   // TODO: strdup ???
  locale_s->variant = NULL;
  locale_s->encoding = encoding; // TODO: strdup ???

  return locale_s;

}

// [allocate]
char* lib_locale_get_locale_name(const char* language, const char* country, const char* encoding) {
  if (!language && !country && !encoding) {
     return NULL;
  }

  char* name = NULL;
  size_t size = 0;

  if (language) {
     size += strlen(language);
  }

  if (country) {
     size += strlen(country);
  }

  if (encoding) {
     size += strlen(encoding);
  }

  if (language && country) {
     size++;   // '_'
  }

  if ((language || country) && encoding) {
     size++;   // '.'
  }

  name = (char*) malloc(size);

  if (language) {
    strcpy(name, language);
  }

  if (country) {
    if (language) {
       strcat(name, "_");
       strcat(name, country);
    } else {   
      strcpy(name, country);
    }
  }

  if (encoding) {
     if (language || country) {
       strcat(name, ".");
       strcat(name, encoding);
     } else {
       strcpy(name, encoding);
     }
  }

  return name;  
}

lib_locale_t* lib_locale_load_locale(int cat) {
  char* name = lib_locale_get_locale(cat); 
  if (!name) {
    return NULL;
  }
  return lib_locale_parse_locale(name);
}

void lib_locale_print_locale(lib_locale_t* locale) {
    if (!locale) {
        printf("Locale: NULL\n");
        return;
    }
    // We use 'strsaf' because if the value is NULL we have the result: '(null)'
    if (locale->name) {
        printf("Locale.name     : %s\n", lib_strsaf(locale->name));
    }    
    printf("Locale.language : %s\n", lib_strsaf(locale->language));
    printf("Locale.script   : %s\n", lib_strsaf(locale->script));
    printf("Locale.country  : %s\n", lib_strsaf(locale->country));
    printf("Locale.variant  : %s\n", lib_strsaf(locale->variant));
    printf("Locale.encoding : %s\n", lib_strsaf(locale->encoding));
}