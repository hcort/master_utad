/*
 * unpack.c
 *
 *  Created on: Sep 8, 2014
 *      Author: pads
 */


#include "unpack.h"

#include<stdlib.h>
#include<stdio.h>
#include <string.h> // for memcpy


// pointers for the list of data
list_node *unpack_head = NULL;
list_node *unpack_current = NULL;

// adds a new packed line to the list
void add_new_packed_line( char* line, int length );


void link_to_external_list( list_node* list_head ){
	if( unpack_head != NULL )
		delete_linked_list( unpack_head );
	unpack_head = list_head;
	unpack_current = unpack_head;
}

list_node* read_packed_file( const char* filename ){

	int n = 0;
	int buffer_length = FIX_LENGTH + 1;
	char line[ buffer_length ];
	// open original text file and pack it
	FILE *fr;            /* file pointer */
	fr = fopen ( (const char*)( filename ), "rt");  /* open the file for reading */
	/* "rt" means open the file for reading text */


	// create the output linked list

	while(fgets(line, buffer_length, fr) != NULL)
	{
		printf( "Line %d\t: %s\n", n, line );
		n++;
		add_new_packed_line( line, buffer_length );
	}


	fclose(fr);  /* close the file */
	//
	return unpack_head;
}


void add_new_packed_line( char* line, int length ){
	// FIXME be careful with '\n
	// length includes '\0'
	int n_length = length-1;
	if( line[ FIX_LENGTH - 1 ] == '\n' )
		n_length = FIX_LENGTH - 1;
	list_node *new = construct_node( n_length );
	memcpy( ((line_data*)new->data)->line_str, line, n_length );
	if( unpack_head == NULL ){
		unpack_head = new;
		unpack_current = unpack_head;
	}else{
		unpack_current->next = new;
		unpack_current = unpack_current->next;
	}
}


list_node*  process_packed_strings( const char* filename ){
	list_node *iterator = unpack_head;

	list_node *compacted_head = NULL;
	list_node *compacted_current = NULL;

	list_node *clear_head = NULL;
	list_node *clear_current = NULL;
	int i = 0; // index for clear array
	int j = 0; // index for interference array
	/*
	 * two iterations -> one to eliminate interferences
	 * 	new interference-free list
	 * 	then, compact this list into a string
	 */
	clear_head = construct_node( FIX_LENGTH );
	clear_current = clear_head;
	int new_length = 0;
	while( iterator != NULL ){
#ifdef PRINT_DBG
		printf("Orig: %s|||\n", ((line_data*)iterator->data)->line_str );
#endif
		for( j = 0; j < FIX_LENGTH-1; j++ ){
			char symbol = ((line_data*)iterator->data)->line_str[j];
			if( symbol != ' ' ){

				((line_data*)clear_current->data)->line_str[i] = symbol;
				if( symbol == '_')
					((line_data*)clear_current->data)->line_str[i] = ' ';
				else if( symbol == '$' ){
					((line_data*)clear_current->data)->line_str[i] = '\n';
					// now, compact this list
					list_node *iterator = clear_head;
					new_length += strlen( ((line_data*)clear_current->data)->line_str );
					list_node *new_compacted = construct_node( new_length );

					char *copy_position = ((line_data*)new_compacted->data)->line_str;
					while( iterator != NULL ){
#ifdef PRINT_DBG
						printf( "Clear fragment: %s|||\n", ((line_data*)iterator->data)->line_str );
#endif
						int curr_length = strlen( ((line_data*)iterator->data)->line_str );
						memcpy( copy_position, ((line_data*)iterator->data)->line_str,
								curr_length );
						copy_position += curr_length;
						// i'll delete the list as I go
						list_node *to_delete = iterator;
						iterator = iterator->next;
						//
						free( ((line_data*)to_delete->data)->line_str );
						free( ((line_data*)to_delete->data) );
						free( to_delete );
					}
					// as I deleted this list, I have to create a new one
					clear_head = construct_node( FIX_LENGTH );
					clear_current = clear_head;
					i = -1; // just after this there's a i++, so i will be 0
#ifdef PRINT_DBG
					printf("Clear string: %s|||\n", ((line_data*)new_compacted->data)->line_str );
#endif
					new_length = 0;
					if( compacted_head == NULL ){
						compacted_head = new_compacted;
						compacted_current = compacted_head;
					}else{
						compacted_current->next = new_compacted;
						compacted_current = compacted_current->next;
					}
				}
				i++;
				if( i == FIX_LENGTH ){
					// make a new node in the list
					new_length += strlen( ((line_data*)clear_current->data)->line_str );
					list_node *new_clear = construct_node( FIX_LENGTH );
					clear_current->next = new_clear;
					clear_current = clear_current->next;
					i = 0;
				}
			}

		}
		iterator = iterator->next;
	}
	if( clear_head != NULL ){ // this remaining list should be only one node
		delete_linked_list( clear_head );
		clear_head = NULL;
		clear_current = NULL;
	}

	if( unpack_head != NULL ){
		delete_linked_list( unpack_head );
	}
	unpack_head = compacted_head;

	save_block_list( filename, ".rst", unpack_head, 0 );

	return unpack_head;
}

