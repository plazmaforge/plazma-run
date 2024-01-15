
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>

#include "syslocale.h"

char* set_default_locale() {
  return setlocale(LC_ALL, "");
}

char* get_locale(int cat) {
  return setlocale(cat, NULL);
}

char* get_locale() {
  return get_locale(LC_CTYPE);
}

int is_empty_locale(char* locale) {
  return locale == NULL || (strcmp(locale, "C") == 0);
}

int equals_locale(char* locale1, char* locale2) {
  if (locale1 == NULL && locale2 == NULL) {
    return 1;
  }
  if (locale1 == NULL || locale2 == NULL) {
    return 0;
  }
  return strcmp(locale1, locale2) == 0;
}

void locale_free(locale_t* locale) {
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

char* parse_encoding(char* locale) {
  if (!locale) {
    return NULL;
  }
  locale_t* locale_s = parse_locale(locale);
  if (!locale_s) {
    return NULL;
  }
  char* encoding = NULL;
  if (locale_s->encoding) {
    encoding = strdup(locale_s->encoding);
  }
  locale_free(locale_s);
  return encoding;
}

locale_t* parse_locale(char* locale) {
  if (!locale) {
    return NULL;
  }

  if (strcmp(locale, "C") == 0) {
    locale_t* locale_s = (locale_t*) malloc(sizeof(locale_t)); // new locale_t();
    locale_s->name = strdup(locale);
    locale_s->language = NULL;
    locale_s->script = NULL;
    locale_s->country = NULL;
    locale_s->variant = NULL;
    locale_s->encoding = NULL;
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
  } else if ((encoding = strchr(temp, '@')) != NULL) {
    *encoding++ = '\0';
  }

  if ((country = strchr(temp, '_')) != NULL) {
    *country++ = '\0';
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
  
  locale_t* locale_s = (locale_t*) malloc(sizeof(locale_t)); // new locale_t();
  locale_s->name = strdup(locale);
  locale_s->language = language;
  locale_s->script = NULL;
  locale_s->country = country;
  locale_s->variant = NULL;
  locale_s->encoding = encoding;

  return locale_s;

}

locale_t* load_locale(int cat) {
  char* name = get_locale(cat /*LC_CTYPE*/); 
  if (!name) {
    return NULL;
  }
  return parse_locale(name);;
}

void printv(const char* label, const char* value) {
    printf(label, value ? value : "");
}

void print_locale(locale_t* locale) {
    if (!locale) {
        printf("Locale: NULL\n");
        return;
    }
    if (locale->name) {
        printv("Locale.name     : %s\n", locale->name);
    }    
    printv("Locale.language : %s\n", locale->language);
    printv("Locale.script   : %s\n", locale->script);
    printv("Locale.country  : %s\n", locale->country);
    printv("Locale.variant  : %s\n", locale->variant);
    printv("Locale.encoding : %s\n", locale->encoding);    
}