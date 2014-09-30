/*
 ============================================================================
 Name        : TreeFormatter.c
 Author      : Héctor
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "htmlparser.h"
#include "xmlparser.h"
#include "utils.h"
#include "list.h"


int main( int argc, char *argv[] ) {
	/*
	 * input: two files
	 * 		document file: HTML-like document
	 * 		style file: CSS-like document with definitions of paragraph styles
	 * 	output: a new HTML-like document with styles applied
	 */

	// get style using libxml
	double_list_node* head = start_parsing_style( argv[2] );
	// get tree using tidy
	start_parsing_html( argv[1], &head );

	free_style_list( &head );
	// get tree using libxml


	return EXIT_SUCCESS;
}
