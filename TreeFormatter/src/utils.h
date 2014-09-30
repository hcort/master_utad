/*
 * utils.h
 *
 *  Created on: Sep 15, 2014
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


char** break_style_string( const char *style_t, int *has_changed );


struct list_node {
	void *data;
	struct list_node *next;
};
typedef struct list_node list_node;

struct double_list_node {
	void *data;
	struct double_list_node *prev;
	struct double_list_node *next;
};

typedef struct double_list_node double_list_node;

#endif /* UTILS_H_ */
