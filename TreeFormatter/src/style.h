/*
 * style.h
 *
 *  Created on: Sep 29, 2014
 *      Author: pads
 */

#ifndef STYLE_H_
#define STYLE_H_

struct node_style {
	char *font;
	char *color;
	char *size;
	char *align;
};
typedef struct node_style node_style;

struct named_style {
	char *name;
	node_style *style;
};

typedef struct named_style named_style;


node_style* copy_style_node( const node_style* orig );

char* create_style_string( node_style* style );


named_style* create_named_style( char* name, char** attr_array );

/**
 * Only pointer handling
 */
int fill_style_with_default( node_style* custom, node_style* def );
/*
 * Uses memcpy to create copies
 */
void fill_values_with_default( named_style* to_fill, named_style* parent );

/*
 * returns 0 if the names are "p" and "p basic", for example
 */
int compare_styles_by_name( const void* sa, const void* sb );

void print_style( const void* data );

void free_named_style( void* data );

int is_default_style( const void* a, const void *b );

/*
 * only detects equals or not-equals
 */
int compare_named_style( const void* a, const void *b );

void print_style_as_css( const void *ns );


#endif /* STYLE_H_ */
