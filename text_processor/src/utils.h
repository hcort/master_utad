/*
 * utils.h
 *
 *  Created on: Sep 7, 2014
 *      Author: pads
 */

#ifndef UTILS_H_
#define UTILS_H_

// command line parsing utilities

typedef enum { PACK, UNPACK, BOTH } progmode;

struct arguments{
	char *filename;
	progmode mode;
};

typedef struct arguments arguments;

arguments* parse_opt( int argc, char *argv[] );

void print_help( char* progname );

typedef struct original_line original_line;


/*
 * Linked list definitions
 */
struct list_node {
	void* data;	// to give support for several data types
	struct list_node *next;
};

typedef struct list_node list_node;

// the data we'll store in this lists
struct line_data {
	char *line_str;
	int length;
};
typedef struct line_data line_data;

#define FIX_LENGTH 32

void delete_linked_list( list_node* head );
int save_block_list( const char* filename, const char* extension, list_node* head_ptr, int add_breaks );

list_node* construct_node( int str_length );


#endif /* UTILS_H_ */
