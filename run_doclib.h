#ifndef PLAZMA_LIB_DOCIB_H
#define PLAZMA_LIB_DOCIB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define LIB_DOC_ENCODING           "utf-8"
#define LIB_DOC_CHARSET            LIB_DOC_ENCODING
#define LIB_DOC_TITLE              NULL
#define LIB_DOC_UNIT               "px"
#define LIB_DOC_MARGIN             "5px"
#define LIB_DOC_FONT_NAME          "Helvetica"
#define LIB_DOC_FONT_STYLE         NULL
#define LIB_DOC_FONT_WEIGHT        NULL
#define LIB_DOC_FONT_SIZE          "12px"

#define LIB_OPT_ENCODING           "encoding"
#define LIB_OPT_CHARSET            "charset"
#define LIB_OPT_TITLE              "title"
#define LIB_OPT_MARGIN             "margin"
#define LIB_OPT_FONT_NAME          "font-name"
#define LIB_OPT_FONT_STYLE         "font-style"
#define LIB_OPT_FONT_WEIGHT        "font-weight"
#define LIB_OPT_FONT_SIZE          "font-size"
#define LIB_OPT_FONT_COLOR         "font-color"
#define LIB_OPT_FONT_FOREGROUND    "font-foreground"
#define LIB_OPT_FONT_BACKGROUND    "font-background"
#define LIB_OPT_WIDTH              "width"
#define LIB_OPT_HEIGHT             "height"

#define LIB_OPT_ENCODING_ID        1000
#define LIB_OPT_CHARSET_ID         1001
#define LIB_OPT_TITLE_ID           2001
#define LIB_OPT_MARGIN_ID          3001
#define LIB_OPT_FONT_NAME_ID       4001
#define LIB_OPT_FONT_STYLE_ID      4002
#define LIB_OPT_FONT_WEIGHT_ID     4003
#define LIB_OPT_FONT_SIZE_ID       4004

#define LIB_OPT_FONT_COLOR_ID      4005
#define LIB_OPT_FONT_FOREGROUND_ID 4006
#define LIB_OPT_FONT_BACKGROUND_ID 4007

#define LIB_OPT_WIDTH_ID           5001
#define LIB_OPT_HEIGHT_ID          5002

typedef struct lib_margin_t {
    const char* value;
    const char* unit;
} lib_margin_t;

typedef struct lib_font_t {
    const char* name;
    const char* style;
    const char* weight;
    const char* size;
    const char* unit;
    const char* color;
    const char* background;
} lib_font_t;

#define LIB_DOC_CONFIG_0      \
    const char* charset;      \
    const char* title;        \
    const char* margin;       \
    const char* margin_unit;  \
    const char* font_name;    \
    const char* font_style;   \
    const char* font_weight;  \
    const char* font_size;    \
    const char* font_unit;    \

#define LIB_DOC_CONTEXT_0     \
    const char* charset;      \
    const char* title;        \
    const char* margin;       \
    const char* margin_unit;  \
    const char* font_name;    \
    const char* font_style;   \
    const char* font_weight;  \
    const char* font_size;    \
    const char* font_unit;    \
    bool use_charset;         \
    bool use_title;           \
    bool use_head;            \
    bool use_margin;          \
    bool use_font_name;       \
    bool use_font_style;      \
    bool use_font_weight;     \
    bool use_font_size;       \
    bool use_font;            \
    bool use_style;           \
    char* data;               \
    size_t size;              \


#define LIB_DOC_CONFIG        \
    const char* charset;      \
    const char* title;        \
    const char* margin;       \
    const char* margin_unit;  \
    lib_font_t* font;         \

#define LIB_DOC_CONTEXT       \
    const char* charset;      \
    const char* title;        \
    const char* margin;       \
    const char* margin_unit;  \
    lib_font_t* font;         \
    bool use_charset;         \
    bool use_title;           \
    bool use_head;            \
    bool use_margin;          \
    bool use_font_name;       \
    bool use_font_style;      \
    bool use_font_weight;     \
    bool use_font_size;       \
    bool use_font;            \
    bool use_style;           \
    char* data;               \
    size_t size;              \

/**
 * DOC Config
 */
typedef struct lib_doc_config_t {
    LIB_DOC_CONFIG
} lib_doc_config_t;

/**
 * DOC Context
 */
typedef struct lib_doc_context_t {
    LIB_DOC_CONTEXT
} lib_doc_context_t;


static bool _isdigit(char c) {
    return c == '0' 
    || c == '1'
    || c == '2'
    || c == '3'
    || c == '4'
    || c == '5'
    || c == '6'
    || c == '7'
    || c == '8'
    || c == '9';
}

static bool lib_is_digit(const char* str) {
    if (!str) {
        return false;
    }
    
    if (*str == '\0') {
        // End string
        return false;
    }

    const char* ptr = str;
    while (*ptr != '\0') {
        if (!_isdigit(*ptr)) {
            return false;
        }
        ptr++;
    }
    return true;
}

static bool lib_is_normal_digit(const char* str) {
    if (!str) {
        return false;
    }

    if (*str == '\0') {
        // End string
        return false;
    }

    if (*str == '0') {
        // Start with '0'
        // '0'                - correct
        // '01', '001', '000' - incorrect
        return *(str + 1) == '\0';
    }

    return lib_is_digit(str);
}

inline static const char* lib_ifs(bool flag, const char* s1, const char* s2) {
    return flag ? s1 : s2;
}

inline static char lib_ifc(bool flag, char c1, char c2) {
    return flag ? c1 : c2;
}

inline static int lib_ifi(bool flag, int i1, int i2) {
    return flag ? i1 : i2;
}

static long lib_ifl(bool flag, long l1, long l2) {
    return flag ? l1 : l2;
}

inline static const char* lib_defs(const char* s1, const char* s2) {
    return s1 ? s1 : s2;
}

////

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

////

static bool _lib_strin(const char* str, const char* find, bool icase) {
    if (!str || !find) {
        return false;
    }

    // if (strcmp(str, find) == 0) {
    //     return true;
    // }

    size_t len = strlen(find);
    if (len == 0) {
        return false;
    }

    const char* s = icase ? lib_stristr(str, find) : lib_strstr(str, find);
    if (!s) {
      return false;
    }

    int start = (int) (s - str);
    int end   = start + len - 1;

    //fprintf(stderr, ">> name     : %s\n", uname);
    //fprintf(stderr, ">> alias    : %s\n", alias);
    //fprintf(stderr, ">> start    : %d\n", start);
    //fprintf(stderr, ">> end      : %d\n", end);
    //fprintf(stderr, ">> alias[%d]: %c\n", start, alias[start]);
    //fprintf(stderr, ">> alias[%d]: %c\n", end, alias[end]);

    bool is_start = false;
    bool is_end = false;

    if (start == 0 || str[start - 1] == ' ') {
      is_start = true;
    }
    if (end == strlen(str) - 1 || str[end + 1] == ' ') {
      is_end = true;
    }

    //fprintf(stderr, ">> is_start : %d\n", is_start);
    //fprintf(stderr, ">> is_end   : %d\n", is_end);

    return is_start && is_end;
}

static bool lib_strin(const char* str, const char* find) {
    return _lib_strin(str, find, false);
}

static bool lib_striin(const char* str, const char* find) {
    return _lib_strin(str, find, true);
}

////

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

int lib_strieq(const char* str1, const char* str2) {
    return lib_stricmp(str1, str2) == 0;
}

////

static const char* lib_doc_unitdef(const char* value) {
    return lib_is_digit(value) ? LIB_DOC_UNIT : "";
}

static bool lib_doc_has_style(const char* str, const char* style) {
    return lib_striin(str, style);
}

////

static bool lib_doc_has_italic(const char* str) {
    return lib_doc_has_style(str, "italic");
}

static bool lib_doc_has_bold(const char* str) {
    return lib_doc_has_style(str, "bold");
}

static bool lib_doc_has_underline(const char* str) {
    return lib_doc_has_style(str, "underline");
}

static bool lib_doc_has_overline(const char* str) {
    return lib_doc_has_style(str, "overline");
}

static bool lib_doc_has_strike(const char* str) {
    return lib_doc_has_style(str, "strike");
}

////

static bool lib_doc_is_bold(const char* str) {
    return lib_stricmp(str, "bold") == 0
    || lib_stricmp(str, "700") == 0;
}

////

static bool lib_doc_has_thick(const char* str) {
    return lib_doc_has_style(str, "thick");
}

static bool lib_doc_has_double(const char* str) {
    return lib_doc_has_style(str, "double");
}

static bool lib_doc_has_word(const char* str) {
    return lib_doc_has_style(str, "word");
}

static bool lib_doc_has_wave(const char* str) {
    return lib_doc_has_style(str, "wave")
    || lib_doc_has_style(str, "wavy");
}

static bool lib_doc_has_dashed(const char* str) {
    return lib_doc_has_style(str, "dashed") 
    || lib_doc_has_style(str, "dash");
}

static bool lib_doc_has_dotted(const char* str) {
    return lib_doc_has_style(str, "dotted") 
    || lib_doc_has_style(str, "dot");
}

////

// dot
static bool lib_doc_has_dot(const char* str) {
    return lib_doc_has_dotted(str);
}

// thdot
static bool lib_doc_has_thdot(const char* str) {
    return lib_doc_has_style(str, "thd") 
    || lib_doc_has_style(str, "thdot")
    || (lib_doc_has_thick(str) && lib_doc_has_dot(str));
}

// dash
static bool lib_doc_has_dash(const char* str) {
    return lib_doc_has_dashed(str);
}

static bool lib_doc_has_dashd(const char* str) {
    return lib_doc_has_style(str, "dashd") 
    || (lib_doc_has_dashed(str) && lib_doc_has_dotted(str));
}

static bool lib_doc_has_dashdd(const char* str) {
    return lib_doc_has_style(str, "dashdd"); // || (lib_doc_has_dashed(str) && lib_doc_has_dotted(str));
}

// thdash
static bool lib_doc_has_thdash(const char* str) {
    return lib_doc_has_style(str, "thdash") 
    || (lib_doc_has_thick(str) && lib_doc_has_dash(str));
}

static bool lib_doc_has_thdashd(const char* str) {
    return lib_doc_has_style(str, "thdashd") 
    || (lib_doc_has_thick(str) && lib_doc_has_dashd(str));
}

static bool lib_doc_has_thdashdd(const char* str) {
    return lib_doc_has_style(str, "thdashdd") 
    || (lib_doc_has_thick(str) && lib_doc_has_dashdd(str));
}

static int lib_doc_font_init(lib_font_t* font, 
    const char* font_name, 
    const char* font_style,  
    const char* font_weight,
    const char* font_size
) {
    if (!font) {
        return 1;
    }
    font->name = font_name;
    font->style = font_style;
    font->weight = font_weight;
    font->size = font_size;
    return 0;
}

static int lib_doc_config_init(lib_doc_config_t* cnf) {
    if (!cnf) {
        return 1;
    }

    cnf->charset     = NULL;
    cnf->title       = NULL;
    cnf->margin      = NULL;
    cnf->margin_unit = NULL;

    cnf->font = NULL;
    return 0;
}

static int lib_doc_context_prepare(lib_doc_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    //ctx->margin_unit = lib_rtf_unitdef(ctx->margin);
    //ctx->font->unit   = lib_rtf_unitdef(ctx->font->size);

    ctx->use_charset        = ctx->charset;
    ctx->use_title          = ctx->title;
    ctx->use_head           = ctx->use_charset || ctx->use_title;

    ctx->use_margin         = ctx->margin;

    if (ctx->font) {
        ctx->use_font_name      = ctx->font->name;
        ctx->use_font_style     = ctx->font->style;
        ctx->use_font_weight    = ctx->font->weight;
        ctx->use_font_size      = ctx->font->size;
        ctx->use_font           = ctx->use_font_name || ctx->use_font_style || ctx->use_font_weight || ctx->use_font_size;
    } else {
        ctx->use_font_name      = false;
        ctx->use_font_style     = false;
        ctx->use_font_weight    = false;
        ctx->use_font_size      = false;
        ctx->use_font           = false;        
    }

    ctx->use_style          = ctx->use_margin || ctx->use_font;

    return 0;

}

static int lib_doc_rgb_parse(const char* str, int val[3]) {
    if (!str) {
        val[0] = 0;
        val[1] = 0;
        val[2] = 0;
        return 1;
    }

    // TODO: Optimize

    //// RGB
    if (lib_strieq(str, "red") || lib_strieq(str, "#FF0000")) {
        val[0] = 255;
        val[1] = 0;
        val[2] = 0;
    } else if (lib_strieq(str, "green") || lib_strieq(str, "#00FF00")) {
        val[0] = 0;
        val[1] = 255;
        val[2] = 0;
    } else if (lib_strieq(str, "blue") || lib_strieq(str, "#0000FF")) {
        val[0] = 0;
        val[1] = 0;
        val[2] = 255;

    //// CST
    } else if (lib_strieq(str, "yellow") || lib_strieq(str, "FFFF00")) {
        val[0] = 255;
        val[1] = 255;
        val[2] = 0;
    } else if (lib_strieq(str, "grey") || lib_strieq(str, "808080")) {
        val[0] = 128;
        val[1] = 128;
        val[2] = 128;
    } else if (lib_strieq(str, "skyblue") || lib_strieq(str, "56B4E9")) {
        val[0] = 86;
        val[1] = 180;
        val[2] = 233;
    } else if (lib_strieq(str, "orange") || lib_strieq(str, "E69F00")) {
        val[0] = 230;
        val[1] = 159;
        val[2] = 0;

    //// W/B
    } else if (lib_strieq(str, "white") || lib_strieq(str, "#FFFFFF")) {
        val[0] = 255;
        val[1] = 255;
        val[2] = 255;
    } else if (lib_strieq(str, "black") || lib_strieq(str, "#000000")) {
        val[0] = 0;
        val[1] = 0;
        val[2] = 0;

    } else {
        val[0] = 0;
        val[1] = 0;
        val[2] = 0;
        return 1;
    }

    return 0;
}

#endif // PLAZMA_LIB_DOCLIB_H