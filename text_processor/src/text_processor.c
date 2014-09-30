/*
 * text_processor.c
 *
 *  Created on: Sep 4, 2014
 *      Author: pads
 */
#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>

#include "utils.h"
#include "pack.h"
#include "unpack.h"


/*
 * generation of "block" files
 * 	input: text file
 * 	output: a "block" file with a fixed number of columns, original line breaks
 * 	replaced with especial code and other interferences
 *
 * restoring text file
 * 	input: "block" file
 * 	output: text file, where the fixed-length rows of "block" have been joined
 * 	interferences deleted and original line breaks added in their place
 *
 * whole program: reads a file with plain text and generates the "block" file,
 * 	then, it process the "block" file again to recreate the original text file
 *
 */


int main( int argc, char *argv[] ){

	arguments *args = parse_opt( argc, argv );
	if( args == NULL ){
		exit(EXIT_FAILURE);
	}
	list_node *pack_head_copy = NULL;
	list_node *unpack_head_copy = NULL;

	if( ( args->mode == PACK ) || ( args->mode == BOTH )){
		/*
		 * this creates the packed file
		 * also returns a struct with the packed strings for
		 * futher processing in BOTH mode
		 */
		pack_head_copy = process_original_file( args->filename );
	}

	if( ( args->mode == BOTH ) || ( args->mode == UNPACK ) ){
		printf("Starting unpack process...\n");
		if( ( pack_head_copy == NULL ) && ( args->mode == BOTH ) ){
			// something bad during the packing process
			printf("Error trying to load packed struct\n");
			free(args);
			return -1;
		}else if( ( pack_head_copy == NULL ) && ( args->mode == UNPACK ) ){
			// i have to read the strings from the file
			printf("Reading from file...\n");
			pack_head_copy = read_packed_file( args->filename );
			if( pack_head_copy == NULL ){
				printf("Error trying to load packed file\n");
				free(args);
				return -1;
			}
		}else{
			// be careful, if you "pass" this pointer, the unpacker shoud manage its
			// memory, not us
			link_to_external_list( pack_head_copy );
			pack_head_copy = NULL;
		}

		printf("Unpacking arrays...\n");
		// now, we start processing the packed strings
		unpack_head_copy = process_packed_strings( args->filename );
		pack_head_copy = NULL; // unpacking process deletes the data in this list
	}

	free( args );
	if( pack_head_copy != NULL ){
		delete_linked_list( pack_head_copy );
		pack_head_copy = NULL;
	}

	if( unpack_head_copy != NULL ){
		delete_linked_list( unpack_head_copy );
		unpack_head_copy = NULL;
	}

	return 0;
}



