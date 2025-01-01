#ifndef PLAZMA_LIB_GETOPT_H
#define PLAZMA_LIB_GETOPT_H

#define no_argument        0
#define required_argument  1
#define optional_argument  2

typedef struct lib_option {

	/* name of long option */
	const char *name;

	/*
	   one of no_argument, 
       required_argument, 
       and optional_argument:

	   whether option takes an argument
	 */
	int has_arg;

	/* if not NULL, set *flag to val when option found */
	int *flag;

	/* if flag not NULL, value to set *flag to; else return value */
	int val;    
} lib_option;

extern char* optinput;
//extern char* optname;
extern char *optarg;

extern int optind; // option index
extern int optopt; // option code
extern int opterr; // print option error flag (for all options)

int lib_getopt(int argc, char* const argv[], const char* optstr);

int lib_getopt_long(int argc, char* const argv[], const char* short_option, const lib_option *long_option, int* long_ind);

int lib_getopt_long_only(int argc, char* const argv[], const char* short_option, const lib_option* long_option, int* long_ind);

#endif // PLAZMA_LIB_GETOPT_H