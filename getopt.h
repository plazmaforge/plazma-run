#ifndef PLAZMA_LIB_ARG_H
#define PLAZMA_LIB_ARG_H

#define no_argument        0
#define required_argument  1
#define optional_argument  2

struct option {

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
};

extern char* optinput;
//extern char* optname;
extern char *optarg;

extern int optind;
extern int opterr; 
extern int optopt;

int getopt(int argc, char* const argv[], const char* optstr);

int getopt_long(int argc, char* const argv[], const char* short_option, const struct option *long_option, int* long_ind);

int getopt_long_only(int argc, char* const argv[], const char* short_option, const struct option* long_option, int* long_ind);

#endif // PLAZMA_LIB_ARG_H