/*
 * utils.c
 *
 *  Created on: Sep 7, 2014
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


void delete_linked_list( list_node* head ){
	//
	list_node *current = head;
	while( current != NULL ){
		list_node *deleted = current;
		line_data *deleted_data = (line_data*)deleted->data;
		current = current->next;
		// delete content inside node, then delete node
		char *str_delete = deleted_data->line_str;
		free( str_delete );
		str_delete = NULL;
		free( deleted_data );
		deleted->data = NULL;
		free( deleted );
	}
}


int save_block_list( const char* filename, const char* extension, list_node* head_ptr, int add_breaks ){
	int name_length = strlen( filename );
	int ext_length = strlen( extension );
	char *new_name = NULL;
	if( name_length == 0 ){
		return -1;
	}else{
		new_name = (char*)malloc( name_length + ext_length + 1 );
		// if not, valgrind complains
		memset( new_name, '\0',  name_length + ext_length + 1  );
		memcpy( new_name, filename, name_length );
		char *offset = new_name + name_length;
		memcpy( offset, extension, ext_length );
	}
//#ifdef PRINT_DBG
	printf("file: %s->%s:\n", filename, new_name );
//#endif
	// save to file
	FILE *block_file;
	block_file = fopen( new_name, "w+");
	if( block_file == NULL )
		return -1;

	list_node *iterator = head_ptr;
	while( iterator != NULL ){
//#ifdef PRINT_DBG
		printf( "to file: %s|||\n", ((line_data*)iterator->data)->line_str );
//#endif
		fprintf( block_file, "%s", ((line_data*)iterator->data)->line_str );
		if( add_breaks != 0 )
			fprintf( block_file, "\n" );
		iterator = iterator->next;
	}

	fclose( block_file );
	free( new_name );
	return 0;
}


list_node* construct_node( int str_length ){
	list_node *new = malloc( sizeof( list_node ) );
	new->next = NULL;
	new->data = malloc( sizeof( line_data ) );
	((line_data*)new->data)->length = 0;
	((line_data*)new->data)->line_str = NULL;
	if( str_length > 0 ){
		((line_data*)new->data)->line_str = malloc( (str_length+1)*sizeof(int) );
		memset( ((line_data*)new->data)->line_str, '\0', str_length+1 );
		((line_data*)new->data)->length = str_length;
	}
	return new;
}
