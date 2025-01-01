#include <stdio.h>
#include <stdlib.h>
//#include <getopt.h>

#include "getopt.h"

// https://azrael.digipen.edu/~mmead/www/Courses/CS180/getopt.html
// https://stackoverflow.com/questions/33469445/passing-negative-numbers-into-getopt

void short_getopt(int argc, char* argv[]) {

    int opt;

    // a - with an argument [optional]
    // b - with an argument
    // X - without an argument

    const char optstr[] = "a:b:XY";

    // ! It doesn't work !
    // first char '-' in 'optstr' - processing non-option arguments in while/loop getopt
    //const char optstr[] = "-:a:b:X";
        
    // first char ':' in 'optstr' - disable the automatic error printing
    // example output error:  'option requires an argument -- a'
    // but if we use first char ':' we have own message (see switch/case ':')
    //const char optstr[] = ":a:b:X"; 

    // ':' after option name - require an argument
    //const char optstr[] = ":a:b:X";

    // ! It doesn't work !
    // '::' after option name - not require an argument
    // but we need use first char ':' in 'optstr' 
    //const char optstr[] = ":a::b:X";
    
    while ((opt = lib_getopt(argc, argv, optstr)) != -1) {
        switch (opt) {
        case 'a':
            printf("Option a has arg: %s\n", optarg ? optarg : "(none)");
            break;
        case 'b':
            printf("Option b has arg: %s\n", optarg);
            break;
        case 'X':
            printf("Option X was provided\n");
            break;
        case 'Y':
            printf("Option Y was provided\n");
            break;
        case '?': 
            printf("Unknown-option: %c, %s\n", optopt, optinput);
            break;
        case ':':
            printf("Missing arg for %c\n", optopt); // work with first char ':' in 'optstr'
            break;
        case 1:
            printf("Non-option arg: %s\n", optarg); // doesn't work with first char '-' in 'optstr'. why?
            break;
        }
    }

    // Get all of the non-option arguments
    if (optind < argc) {
        printf("Non-option args:\n");
        while (optind < argc) {
            printf("%s\n", argv[optind++]);
        }            
    }

}

void long_getopt(int argc, char* argv[]) {

    int c;

while (1) 
  {
      int option_index = 0;

    //   static struct lib_option long_options[] = 
    //   {
    //       {"add",     required_argument, NULL,  1000 },
    //       {"append",  no_argument,       NULL,  1000 },
    //       {"delete",  required_argument, NULL,  1000 },
    //       {"verbose", no_argument,       NULL,  1000 },
    //       {"create",  required_argument, NULL,  1000 },
    //       {"file",    optional_argument, NULL,  1000 },
    //       {NULL,      0,                 NULL,  0 }
    //   };

    //   static struct lib_option long_options[] = 
    //   {
    //       {"add",     required_argument, NULL,  'A' },
    //       {"append",  no_argument,       NULL,  'P' },
    //       {"delete",  required_argument, NULL,  'D' },
    //       {"verbose", no_argument,       NULL,  'V' },
    //       {"create",  required_argument, NULL,  'C' },
    //       {"file",    optional_argument, NULL,  'F' },
    //       {NULL,      0,                 NULL,  'X' }
    //   };

    //   static struct lib_option long_options[] = 
    //   {
    //       {"add",     required_argument, 0,  'a' },
    //       {"append",  no_argument,       0,  'b' },
    //       {"delete",  required_argument, 0,  'd' },
    //       {"verbose", no_argument,       0,  'v' },
    //       {"create",  required_argument, 0,  'c' },
    //       {"file",    optional_argument, 0,  'f' },
    //       {NULL,      0,                 0,  0 }
    //   };

      static struct lib_option long_options[] = 
      {
          {"add",     required_argument, 0,  1000 },
          {"append",  no_argument,       0,  2000 },
          {"delete",  required_argument, 0,  3000 },
          {"verbose", no_argument,       0,  4000 },
          {"create",  required_argument, 0,  5000 },
          {"file",    optional_argument, 0,  6000 },
          {NULL,      0,                 0,  0 }
      };

    //   static struct option long_options[] = 
    //   {
    //       {"add",     required_argument, NULL,  1000 },
    //       {"append",  no_argument,       NULL,  2000 },
    //       {"delete",  required_argument, NULL,  3000 },
    //       {"verbose", no_argument,       NULL,  4000 },
    //       {"create",  required_argument, NULL,  5000 },
    //       {"file",    optional_argument, NULL,  6000 },
    //       {NULL,      0,                 NULL,  0 }
    //   };

      //c = lib_getopt_long_only(argc, argv, "-:abc:d::", long_options, &option_index);
      //c = lib_getopt_long_only(argc, argv, "-:012", long_options, &option_index);
      c = lib_getopt_long_only(argc, argv, "", long_options, &option_index);
      if (c == -1)
           break;

      if (c >= 1000 && c<= 6000) {
      //if (c == 1000) {  
          printf("A: Long option %s", long_options[option_index].name);
          if (optarg) {
            printf(" = %s", optarg);
          }             
          printf("\n");
          continue;
      }

      switch (c) 
      {
        // case 0:
        //   printf("Long option %s", long_options[option_index].name);
        //   if (optarg)
        //      printf(" with arg %s", optarg);
        //   printf("\n");
        //   break;

        case 1:
          printf("A: Regular argument '%s'\n", optarg); /* non-option arg */
          break;

    //     case 'a':
    //       //printf("option a\n");
    //       printf("Long option %s", long_options[option_index].name);
    //       if (optarg)
    //          printf(" with arg %s", optarg);
    //       printf("\n");
    //       break;

    //    case 'b':
    //       //printf("option b\n");
    //       printf("Long option %s", long_options[option_index].name);
    //       if (optarg)
    //          printf(" with arg %s", optarg);
    //       printf("\n");
    //       break;

    //     case 'c':
    //       //printf("option c with value '%s'\n", optarg);
    //       printf("Long option %s", long_options[option_index].name);
    //       if (optarg)
    //          printf(" with arg %s", optarg);
    //       printf("\n");
    //       break;

    //     case 'd':
    //       //printf("option d with value '%s'\n", optarg ? optarg : "NULL");
    //       printf("Long option %s", long_options[option_index].name);
    //       if (optarg)
    //          printf(" with arg %s", optarg);
    //       printf("\n");
    //       break;

    //     case 'f':
    //       //printf("option d with value '%s'\n", optarg ? optarg : "NULL");
    //       printf("Long option %s", long_options[option_index].name);
    //       if (optarg)
    //          printf(" with arg %s", optarg);
    //       printf("\n");
    //       break;

    //     case 'v':
    //       //printf("option d with value '%s'\n", optarg ? optarg : "NULL");
    //       printf("Long option %s", long_options[option_index].name);
    //       if (optarg)
    //          printf(" with arg %s", optarg);
    //       printf("\n");
    //       break;

        case '?':
          //printf("A: Unknown option %d\n", optopt);
          //printf("Unknown option %c\n", optopt);
          //printf("Unknown option '%s'\n", long_options[option_index].name);
          printf("A: Unknown option %s\n", optinput);
          break;

        case ':':
          //printf("A: Missing option for %d\n", optopt);
          //printf("Missing option for %c\n", optopt);
          printf("A: Missing option for %s\n", long_options[option_index].name);
          break;

        default:
          printf("A: ?? getopt returned character code 0%o ??\n", c);
       }

           // Get all of the non-option arguments
  }

    if (optind < argc) {
        printf("Non-option args:\n");
        while (optind < argc) {
            printf("%s\n", argv[optind++]);
        }            
    }


}

int main(int argc, char* argv[]) {
    //short_getopt(argc, argv);
    long_getopt(argc, argv);
}