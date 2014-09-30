/*
 * utils.c
 *
 *  Created on: Sep 15, 2014
 *      Author: pads
 */



#include "utils.h"

#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include<string.h>
#include<argp.h>


arguments* parse_opt( int argc, char *argv[] ){
	// using getopt
	int opt;
	int b = 10;
	char *endptr;
	int number;

	arguments *args = (arguments*)malloc( sizeof( arguments ) );
	args->mode = BOTH; //default
	// composition of valid options
	// the options before the ":" take arguments
	char valid_opt[] = ":f:m:h";
	while( (opt = getopt(argc, argv, valid_opt )) != -1)
	{
		switch (opt)
		{
		case 'h':
			print_help( argv[0] );
			free( args );
			return NULL;
		break;
		case 'f':
			// getopt checks for empty flag if is the last one but doesn't check for something like -f -n, lacking argument for f
			args->filename = argv[ optind - 1 ];
			if( args->filename[0] == '-' ){
				fprintf( stderr, "Incorrect -f usage, needs a file name\n" );
				free( args );
				return NULL;
			}
			fprintf( stdout, "Parameter filename : %s\n", args->filename );
		break;
		case 'm':
			number = (int)strtol( argv[ optind - 1 ], &endptr, b );
			// error checking
			errno = 0;    /* To distinguish success/failure after call */

			/* Check for various possible errors */
			if ((errno == ERANGE && (number == LONG_MAX || number == LONG_MIN))
					|| (errno != 0 && number == 0)) {
				perror("strtol"); //strtol failed somehow
				free( args );
				return NULL;
			}
			// not a number (text or maybe another flag) or bad data after number
			if( (endptr == argv[ optind ] ) || (*endptr != '\0')  ) {
				fprintf(stderr, "Incorrect -n usage, needs a number [ 0, 1, 2 ]\n");
				free( args );
				return NULL;
			}
			if( number == 0 )
				args->mode = BOTH;
			else if( number == 1 )
				args->mode = PACK;
			else if( number == 2 )
				args->mode = UNPACK;
			else{
				fprintf(stderr, "Incorrect -n usage, needs a number [ 0, 1, 2 ]\n");
				free( args );
				return NULL;
			}
		break;
		case ':':
			fprintf(stderr, "flag -%c is missing parameters\n", optopt );
			free( args );
			return NULL;
		break;
		default:
			fprintf(stderr, "flag -%s not recognized\n", argv[ optind -1 ] );
			fprintf(stderr, "Use %s -h for help\n", argv[0]);
			free( args );
			return NULL;
		}
	}
	return args;
}

void print_help( char* progname ){
	fprintf( stdout, "%s usage\n", progname );
	fprintf( stdout, "\t-f filename : input file\n");
}

char** break_style_string( const char *style_t, int *has_changed ){

	// TODO break the attribute into its values -> style="color:blue;font-size:xx-small;"
	char *ptr_prop = style_t;
	char *ptr_val = strchr( style_t, ':' );
	char *end = strchr( style_t, '\0' );
	char **values_array = malloc( 4*sizeof( char* ) );
	memset( values_array, 0, 4*sizeof( char* ) );
	char** value_changed = NULL;
	*has_changed = 0;
	/*
struct node_style {
	char *font;		index = 0
	char *color;	index = 1
	char *size;		index = 2
	char *align;	index = 3
};*/
	while( ptr_prop != NULL ){
		if( *ptr_prop == ';' )
			ptr_prop++;
		int last_change = *has_changed;
		if( strncmp( ptr_prop, "font-family", ptr_val - ptr_prop ) == 0 ){
			//printf("Defining font-face prop\n");
			value_changed = &values_array[0];
			*has_changed += 1;
		}
		if( strncmp( ptr_prop, "color", ptr_val - ptr_prop ) == 0 ){
			//printf("Defining color prop\n");
			value_changed = &values_array[1];
			*has_changed += 4;
		}
		if( strncmp( ptr_prop, "font-size", ptr_val - ptr_prop ) == 0 ){
			//printf("Defining font-size prop\n");
			value_changed = &values_array[2];
			*has_changed += 2;
		}
		if( strncmp( ptr_prop, "text-align", ptr_val - ptr_prop ) == 0 ){
			//printf("Defining align prop\n");
			value_changed = &values_array[3];
			*has_changed += 8;
		}
		//printf("Attr: %s\n", ptr_prop );

		char *ptr_prop_next = strchr( ptr_val, ';' );
		// end condition: either NULL "last_prop:last_value"
		if( ptr_prop_next == NULL ){

			//printf("Last attribute, ends without ;\n");
			ptr_prop = end;
		}else{
			ptr_prop = ptr_prop_next;
			/*
			if( ptr_prop_next == (end-1) ){
				printf("Last attribute, ends with ;\n");
			}
			// */
		}
		if( ptr_prop != NULL ){
			if( last_change != *has_changed ){
				// if this is the last value we have to be careful about strings ending without ;
				*value_changed = malloc( ptr_prop-ptr_val );
				memset( *value_changed, 0, ptr_prop-ptr_val );
				memcpy( *value_changed, ptr_val+1, ptr_prop-ptr_val-1 );
				/*
				printf("Value: %s\n", *value_changed );
				printf("\n");
				// */
				ptr_val = strchr( ptr_prop, ':' );
			}
		}
		if( ptr_prop >= end - 1 )
			ptr_prop = NULL;
	}
	return values_array;
}

