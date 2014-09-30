/*
 * pack.c
 *
 *  Created on: Sep 7, 2014
 *      Author: pads
 */


#include "pack.h"
#include "utils.h"

#include<stdlib.h>
#include<stdio.h>
#include <string.h> // for memcpy


// pointers for the list of data
list_node *head = NULL;
list_node *current = NULL;

int reconstruct_strings();
int make_block_list();

list_node* process_original_file( const char* filename ){
	int n = 0;
	int buffer_length = 80;
	char line[ buffer_length ];
	// open original text file and pack it
	FILE *fr;            /* file pointer */
	fr = fopen ( (const char*)( filename ), "rt");  /* open the file for reading */
	/* "rt" means open the file for reading text */


	// create the output linked list

	while(fgets(line, buffer_length, fr) != NULL)
	{
//#ifdef PRINT_DBG
		printf( "Line %d\t: %s\n", n, line );
//#endif
		n++;
		add_new_line( line, buffer_length );
	}


	fclose(fr);  /* close the file */
	//
	return start_packing( filename );
}



void delete_list_from_packing(){
	//
	current = head;
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
	head = NULL;
}

/**
 * TODO we have two methods that do the same here and in unpack.c
 * 	how could we do this in utils, keeping the head/current pointers still in each file?
 */
int add_new_line( const char* buffer, int buffer_length ){

	// create new data entry
	// construct_node handles the space for '\0'
	list_node *new = construct_node( buffer_length -1 );
	memcpy( ((line_data*)new->data)->line_str, buffer, sizeof( char ) * buffer_length );

	if( head == NULL ){
		// first line added
		head = new;
		current = head;
	}else{
		current->next = new;
		current = new;
	}
	return 0; // everything was OK
}


list_node* start_packing( const char *filename ){
	// head points to a list of fixed-length strings
	reconstruct_strings();
	// now, it points to the reconstructed strings list
	// start processing the reconstructed lines into new fixed-length ones
	make_block_list();
	// now, head points to the list with new fixed-length strings with interferences
	save_block_list( filename, ".blk", head, 1 );

	return head;
}


int reconstruct_strings(){
	list_node *iterator = head;
	if( iterator == NULL ){
		printf( "Error packing data: empty list\n" );
		return -1;
	}

	// if last line doesn't have a '\n' at the end and strlen < length (bad file formatting)
	int line_len = strlen( ((line_data*)(current->data))->line_str );
	if( line_len < ((line_data*)(current->data))->length ){
		// manually put the '\n' character
		((line_data*)(current->data))->line_str[ line_len ] = '\n';
	}

	int n = 0;
	// looping through the linked list
	list_node *new_head = NULL;
	list_node *new_current = NULL;
	list_node *advanced = iterator;
	int original_length = 0;
	//char *original_line = NULL;
	int found = 0;
	while( iterator->next != NULL ){
		// I don't know how long the original lines were, so I have to find out
		advanced = iterator;
		original_length = 0;
		//original_line = NULL;
		found = 0;
		char *end_line = NULL;
		while( ( advanced->next != NULL ) && ( found == 0 ) ){
			// Iterate through the list until I find the first '\n'
			if( ( end_line = strchr(
					((line_data*)(advanced->data))->line_str , '\n' ) ) != NULL ){
				// found!
				found = 1;
			}else{
				// next item
				original_length += ((line_data*)(advanced->data))->length;
				advanced = advanced->next;
			}
		}
		if( ( found == 0 ) && ( advanced->next == NULL ) ){
			// the loop got to the end of the list, leaving "advanced" untreated
			if( ( end_line = strchr(
					((line_data*)(advanced->data))->line_str , '\n' ) ) != NULL )
				found = 1;
		}

		//
		//
		if( ( found == 0 ) || ( end_line == NULL ) ){
			printf("Something bad happened, nothing found\n");
			// garbage collecting
			return -1;
		}else if( ( found == 1 ) && ( end_line != NULL ) ){
			// length by pointer offset
			int offset = end_line - ((line_data*)(advanced->data))->line_str;
			original_length += offset + 1;
			list_node *new_node = construct_node( original_length );
			char* copy_ptr = ((line_data*)new_node->data)->line_str;
			// now I have to catch up with iterator
			// we know advanced-next is not NULL, condition on while loop
			while( iterator != advanced ){
				printf("\t%s|||\n", ((line_data*)(iterator->data))->line_str );
				memcpy( copy_ptr,
						((line_data*)(iterator->data))->line_str,
						((line_data*)(iterator->data))->length );
				copy_ptr += ((line_data*)(advanced->data))->length;
				iterator = iterator->next;
			}
			// last element
			memcpy( copy_ptr,
				((line_data*)(iterator->data))->line_str,
				offset + 1 );

			printf("\t%s|||\n", ((line_data*)(iterator->data))->line_str );
//#ifdef PRINT_DBG
			printf("original(%d): %s\n-_-\n", original_length, ((line_data*)new_node->data)->line_str );
//#endif
			if( new_head == NULL ){
				new_head = new_node;
				new_current = new_head;
			}else{
				new_current->next = new_node;
				new_current = new_current->next;
			}
		}
		n++;

		/*
		 * now iterator == advanced
		 * two situations:
		 * 		advanced->next == NULL. end of processing, nothing left to do
		 * 		advanced->next != NULL. point iterator to the beginning of next sentence
		 */

		if( iterator->next != NULL ){
			iterator = iterator->next;
		}
	}

	// lines reconstructed, now I can delete the old fixed-length listems: %d\n", list_items );

	delete_linked_list( head );
	// now the list I want to use is the one with the reconstructed lines
	head = new_head;
	current = new_head;
	return 0;
}

/*
 * turns the string list into a new fixed-length list
 */
int make_block_list(){

	int n = 0;
	int i = 0; int j = 0;

	list_node *new_head = construct_node( FIX_LENGTH -1 );
	list_node *new_current = new_head;

	/*
	 * With this kind of loops we'll get out as soon as we get to the last element so,
	 * 	if we want to treat it, we have to do it manually
	 */
	while( current != NULL ){
//#ifdef PRINT_DBG
		printf("Elem %d: %s\n", n, ((line_data*)current->data)->line_str );
//#endif
		int current_length = ((line_data*)current->data)->length;
		// create random seed
		unsigned int seed;
		FILE* urandom = fopen("/dev/urandom", "r");
		fread(&seed, sizeof(int), 1, urandom);
		fclose(urandom);
		srand(seed);
		int next_interference = i + ( ( rand() % 15 ) + 1 )% current_length;
		int interference_length = ( 1+ ( rand()%2 ) )*( 1 + (rand()%5 ) );
		// we'll process this element character by character,
		while( i < current_length ){
			//we replace ' ' for '_' and add blank spaces ' ' as new interferences
			if( i == next_interference ){
				// put interference
				int k = 0;
				for( k = 0; k < interference_length; k++ ){
					((line_data*)(new_current->data))->line_str[ j ] = ' ';
					j++;
					if( j == ( FIX_LENGTH -1 ) ){
						((line_data*)(new_current->data))->line_str[j] = '\0';
						// create new row
						list_node *new_node = construct_node( FIX_LENGTH -1 );
						new_current->next = new_node;
						new_current = new_current->next;
						j = 0;
					}
				}
				// create new interference
				next_interference = i + ( ( rand() % 15 ) + 1 )% current_length;
				interference_length = ( 1+ ( rand()%2 ) )*( 1 + (rand()%5 ) );
			}
			((line_data*)(new_current->data))->line_str[ j ] = ((line_data*)current->data)->line_str[ i ];
			if( ((line_data*)(new_current->data))->line_str[ j ] == '\n' )
				((line_data*)(new_current->data))->line_str[ j ] = '$';
			if( ((line_data*)(new_current->data))->line_str[ j ] == ' ' )
				((line_data*)(new_current->data))->line_str[ j ] = '_';
			j++;
			if( j == ( FIX_LENGTH -1 ) ){
				((line_data*)(new_current->data))->line_str[j] = '\0';
				// create new row
				list_node *new_node = construct_node( FIX_LENGTH -1 );
				new_current->next = new_node;
				new_current = new_current->next;
				j = 0;
			}
			i++;
		}

		if( ( current->next == NULL )  && ( j < ( FIX_LENGTH - 1 ) ) ){
			// last line '\n' doesn't fills a fixed-length line
			while( j < FIX_LENGTH -1 ){
				((line_data*)new_current->data)->line_str[j] = ' ';
				j++;
			}
			((line_data*)new_current->data)->line_str[j] = '\0';
		}

		current = current->next;
		if( current != NULL ){
			current_length = ((line_data*)current->data)->length;
			i = 0;
		}
		n++;
	}

	// empty the previous list, next step is expecting a fixed-length array
	delete_linked_list( head );
	//
	head = new_head;
	current = head;


	return 0;
}

