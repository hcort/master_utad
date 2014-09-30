/*
 * xmlparser.c



 *
 *  Created on: Sep 23, 2014
 *	  Author: pads
 */


/**
 * section: Tree
 * synopsis: Navigates a tree to print element names
 * purpose: Parse a file to a tree, use xmlDocGetRootElement() to
 *		  get the root element, then walk the document and print
 *		  all the element name in document order.
 * usage: tree1 filename_or_URL
 * test: tree1 test2.xml > tree1.tmp && diff tree1.tmp $(srcdir)/tree1.res
 * author: Dodji Seketeli
 * copy: see Copyright for the status of this software.
 */
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "xmlparser.h"
#include "utils.h"
#include "string.h"
#include "list.h"
#include "style.h"


void put_style_in_list( char *name, char **new_style, double_list_node **list );




void free_style_list( double_list_node **list ){
	delete_list( list, free_named_style );

}

/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
void print_element_names(xmlNode * a_node, int ntabs, double_list_node** head )
{
	xmlNode *cur_node = NULL;
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if( cur_node->type == XML_ELEMENT_NODE ){
			// look for id, if it exists
			char *id = NULL;
			if( cur_node->properties != NULL ){
				if( ( cur_node->properties->type == XML_ATTRIBUTE_NODE ) &&
					( cur_node->properties->name != NULL ) && ( cur_node->properties->children != NULL ) &&
					( cur_node->properties->children->content != NULL  ) ){
					if( strcmp( (char*)(cur_node->properties->name), "id" ) == 0 )
						id = (char*)(cur_node->properties->children->content);
				}
			}

			// look for content
			char *style_string = NULL;
			if( ( cur_node->children != NULL ) && ( cur_node->children->content != NULL ) &&
					( strlen( (char*)(cur_node->children->content) ) > 2 ) ){
				//printf("%*s node content: %s\n", ntabs*2, "\t", cur_node->children->content );
				style_string = (char*)(cur_node->children->content);
			}
			if( ( id != NULL ) || ( style_string != NULL ) ){
				int length = strlen( (char*)cur_node->name ) + 1;
				int blank = length;
				if( id != NULL ){
					// <p id="foo" -> "p foo" ( 1 extra blank character, null terminator
					length += strlen( id ) + 1;
				}
				char *name = malloc( length );
				memset( name, '\0', length );
				memcpy( name, cur_node->name, blank-1 );
				if( id != NULL ){
					name[ blank -1 ] = ' ';
					memcpy( (name + blank ), id, length - blank );
				}
				/*
				printf("%*s This node defines a style: %s -> %s\n", ntabs, "\t",
						new_style->name, style_string );
				// */
				// break style string into components
				int has_changed;
				char **style_array = break_style_string( style_string, &has_changed );

				named_style *new_style = create_named_style( name, style_array );

				// we'll search for "default" style for the kind of node
				// before inserting or we'll find ourselves
				double_list_node *parent = find_item( head, new_style, compare_styles_by_name );
//				//
//				printf("inserting style %s : font=%s, color=%s, size=%s, align=%s\n",
//						new_style->name,
//						( new_style->style->font != NULL ) ? new_style->style->font : "_" ,
//						( new_style->style->color != NULL ) ? new_style->style->color : "_" ,
//						( new_style->style->size != NULL ) ? new_style->style->size : "_" ,
//						( new_style->style->align != NULL ) ? new_style->style->align : "_" );
				if( parent != NULL ){
//					printf("\tInherits %s : font=%s, color=%s, size=%s, align=%s\n",
//							(( named_style* )parent->data)->name,
//							( ( (( named_style* )parent->data)->style->font != NULL ) ? (( named_style* )parent->data)->style->font : "_" ),
//							( ( (( named_style* )parent->data)->style->color != NULL ) ? (( named_style* )parent->data)->style->color : "_" ),
//							( ( (( named_style* )parent->data)->style->size != NULL ) ? (( named_style* )parent->data)->style->size : "_" ),
//							( ( (( named_style* )parent->data)->style->align != NULL ) ? (( named_style* )parent->data)->style->align : "_" ));
					fill_values_with_default( new_style, ( named_style* )parent->data );
//					printf("\tfinal value : font=%s, color=%s, size=%s, align=%s\n",
//							( new_style->style->font != NULL ) ? new_style->style->font : "_" ,
//							( new_style->style->color != NULL ) ? new_style->style->color : "_" ,
//							( new_style->style->size != NULL ) ? new_style->style->size : "_" ,
//							( new_style->style->align != NULL ) ? new_style->style->align : "_" );
				}
				//  */
				put_item( head, new_style );
				//put_style_in_list( name, style_array, head );
				// delete array
				int i;
				for( i = 0; i < 4; i++ ){
					if( style_array[i] != NULL ) free( style_array[i] );
				}
				free( style_array );
			}
		}

		print_element_names(cur_node->children, ntabs*4, head );
	}
}


/**
 * Simple example to parse a file called "file.xml",
 * walk down the DOM, and print the name of the
 * xml elements nodes.
 */
double_list_node* start_parsing_style( const char *filename )
{
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	/*
	 * this initialize the library and check potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */
	LIBXML_TEST_VERSION

	/*parse the file and get the DOM */
	doc = xmlReadFile( filename, NULL, 0);

	if (doc == NULL) {
		printf("error: could not parse file %s\n", filename );
	}

	/*Get the root element node */
	root_element = xmlDocGetRootElement(doc);

	double_list_node* head = NULL;
	print_element_names(root_element, 1, &head );

	print_list( &head, print_style );

	/*free the document */
	xmlFreeDoc(doc);

	/*
	 *Free the global variables that may
	 *have been allocated by the parser.
	 */
	xmlCleanupParser();

	return head;
}

void put_style_in_list( char *name, char **attr_array, double_list_node **list ){
	named_style *new_style = create_named_style( name, attr_array );
	put_item( list, new_style );
}
