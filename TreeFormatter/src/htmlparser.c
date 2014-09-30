/*
 * htmlparser.c
 *
 *	Created on: Sep 15, 2014
 *			Author: pads
 */



#include <stdio.h>
#include <tidy/tidy.h>
#include <tidy/buffio.h>
#include <curl/curl.h>
#include<string.h>

#include "htmlparser.h"
#include "list.h"
#include "style.h"
#include "fileoutput.h"

list_node *stack_head = NULL;

double_list_node *list_head = NULL;
int unknown_index = 0;


void dumpNode(TidyDoc doc, TidyNode tnod, int indent, ctmbstr node_name );
uint write_cb(char *in, uint size, uint nmemb, TidyBuffer *out);

int update_current_style( TidyNode node  );
int parse_style_string( TidyNode style_node, int first );

int compare_named_style( const void* a, const void *b );
int is_default_style( const void* a, const void *b );

void put_style_in_stack( int has_changed,
		ctmbstr face_t, ctmbstr size_t, ctmbstr color_t, ctmbstr align_t, int first );

void pop_stack();

//void dump_style_list_as_css( double_list_node** list_head );
//void print_style_as_css( const void *named_style );

void process_named_node( ctmbstr name, const char* indent );

int start_parsing_html( const char* file, double_list_node** xml_list ){

	int argc = 2;
	TidyDoc tdoc;
	//TidyBuffer docbuf = {0};
	TidyBuffer tidy_errbuf = {0};

	list_head = get_list_head( xml_list );

	int err = 0;
	if ( argc == 2) {
		FILE *stream;
		char *contents;
		int fileSize = 0;

		printf("Opening: %s\n", file );
		stream = fopen( file, "rb");
		if( stream == NULL ){
			printf("Error reading file\n");
			return -1;
		}

		//Seek to the end of the file to determine the file size
		fseek(stream, 0L, SEEK_END);
		fileSize = ftell(stream);
		fseek(stream, 0L, SEEK_SET);

		//Allocate enough memory (add 1 for the \0, since fread won't add it)
		contents = malloc(fileSize+1);

		//Read the file
		size_t size=fread(contents,1,fileSize,stream);
		contents[size]=0; // Add terminating zero.

		//Print it again for debugging
		printf("Read %s\n", contents);
		tdoc = tidyCreate();
		tidyOptSetBool(tdoc, TidyForceOutput, yes); /* try harder */
		tidyOptSetInt(tdoc, TidyWrapLen, 4096);
		tidySetErrorBuffer( tdoc, &tidy_errbuf );

		if ( !err ) {
			err = tidyParseString( tdoc, contents );
			if ( err >= 0 ) {
				err = tidyCleanAndRepair(tdoc); /* fix any problems */
				if ( err >= 0 ) {
					err = tidyRunDiagnostics(tdoc); /* load tidy error buffer */
					if ( err >= 0 ) {

						dumpNode( tdoc, tidyGetRoot(tdoc), 0, "" ); /* walk the tree */

						//print_headers();
						//dump_style_list_as_css( &list_head );
						dump_temp_into_output( &list_head );
						fprintf(stderr, "%s\n", tidy_errbuf.bp); /* show errors */
					}
				}
			}
		}
		free( contents );
		fclose( stream );

		tidyBufFree(&tidy_errbuf);
		tidyRelease(tdoc);
		return(err);

	}
	return(0);
}

/* curl write callback, to fill tidy's input buffer...	*/
uint write_cb(char *in, uint size, uint nmemb, TidyBuffer *out)
{
	uint r;
	r = size * nmemb;
	tidyBufAppend( out, in, r );
	return(r);
}

/* Traverse the document tree */
void dumpNode(TidyDoc doc, TidyNode tnod, int indent, ctmbstr node_name )
{

	char indent_str[50] = {0};
	memset( indent_str, '\t', indent );
	TidyNode child;
	int style_change = 0;
	int i = 0;
	ctmbstr new_node_name = node_name;
	for ( child = tidyGetChild(tnod); child; child = tidyGetNext(child) )
	{
		ctmbstr name = tidyNodeGetName( child );
		style_change = update_current_style( child );

		if ( name && !tidyNodeIsFONT( child ) )	{
			process_named_node( name, indent_str );
		}
		else if( !name && !tidyNodeIsFONT( child ) ) {
			/* if it doesn't have a name, then it's probably text, cdata, etc... */
			TidyBuffer buf;
			tidyBufInit(&buf);
			tidyNodeGetText(doc, child, &buf);
			put_text_in_ouput_file( (const char*)buf.bp, indent_str+1 );
			tidyBufFree(&buf);
		}
		dumpNode( doc, child, indent + 1, new_node_name ); /* recursive */

		if( name && !tidyNodeIsFONT( child ) ){
			close_tag_in_ouput_file( name, indent_str );
		}
		if( style_change != 0 ){
			pop_stack();
			style_change = 0;
		}
		i++;
	}
	//free( current_style );
	//close_tag_in_ouput_file( tidyNodeGetName( tnod ) );
}

void pop_stack(){

	// pop from stack
	list_node *ptr = stack_head;
	stack_head = stack_head->next;
	// delete all strings
	node_style *style = (node_style*)(ptr->data);
	if( style->align != NULL ) free( style->align );
	if( style->color != NULL ) free( style->color );
	if( style->font != NULL ) free( style->font );
	if( style->size != NULL ) free( style->size );
	free( (node_style*)(ptr->data) );
	free( ptr );
	/*
	if( stack_head != NULL ){
		printf("Restoring style: %s, %s, %s, %s\n",
			((node_style*)(stack_head->data))->font, ((node_style*)(stack_head->data))->color,
			((node_style*)(stack_head->data))->size, ((node_style*)(stack_head->data))->align );
	}else{
		printf("Restoring default style\n");
	}
	*/
}

int update_current_style( TidyNode node ){
	TidyNode align_node = NULL;
	TidyNode style_node = NULL;

	if( tidyAttrGetById( node, TidyAttr_ALIGN ) != NULL )	align_node = node;
	else if( tidyAttrGetById( node, TidyAttr_STYLE ) != NULL )	style_node = node;
	if( ( align_node == NULL ) && ( style_node == NULL ) )
		return 0;

	int has_changed = 0;

	//if( current_style == NULL ){
	node_style *current_style = NULL;
	int first = 0;
	if( stack_head == NULL ){
		/*
		 * if we have no stack, any font definition is a change, I'll create
		 * 	an empty style in stack head
		 */
		stack_head = malloc( sizeof( list_node ) );
		stack_head->data = malloc( sizeof( node_style ) );
		memset( stack_head->data, 0, sizeof( node_style ) );
		stack_head->next = NULL;
		first = 1;
	}
	if( style_node != NULL ){
		has_changed = parse_style_string( style_node, first );
		return has_changed;
	}

	current_style = (node_style*)(stack_head->data);
	if( align_node != NULL ) {

		TidyAttr align = tidyAttrGetById( align_node, TidyAttr_ALIGN );

		ctmbstr align_t = tidyAttrValue( align );
		// check if new align is different
		if( ( current_style->align == NULL ) || ( strcasecmp( current_style->align, align_t ) != 0 ) ){
			has_changed += 8;
			put_style_in_stack( has_changed, NULL, NULL, NULL, align_t, first );
		}
	}

	return has_changed;
}

int parse_style_string( TidyNode style_node, int first ){
	int has_changed = 0;
	ctmbstr style_t = NULL;
	TidyAttr style = tidyAttrGetById( style_node, TidyAttr_STYLE );
	style_t = tidyAttrValue( style );
	/*
	//printf( "\t\tSTYLE ATTR:\t%s", tidyAttrName( style ));
	tidyAttrValue(style)?printf("=\"%s\" \n", style_t ):printf("_\n"); // */
	// break the attribute into its values -> style="color:blue;font-size:xx-small;"
	char **values_retrieved = break_style_string( style_t, &has_changed );

	/*
struct node_style {
	char *font;		index = 0
	char *color;	index = 1
	char *size;		index = 2
	char *align;	index = 3
};*/
	if( values_retrieved != NULL ){
		put_style_in_stack( has_changed, values_retrieved[0], values_retrieved[1], values_retrieved[2], values_retrieved[3], first );
		// I had created some local copies
		if( values_retrieved[0] != NULL ) free( values_retrieved[0] );
		if( values_retrieved[1] != NULL ) free( values_retrieved[1] );
		if( values_retrieved[2] != NULL ) free( values_retrieved[2] );
		if( values_retrieved[3] != NULL ) free( values_retrieved[3] );
		free( values_retrieved );
	}
	return has_changed;
}

void put_style_in_stack( int has_changed, ctmbstr face_t, ctmbstr color_t, ctmbstr size_t, ctmbstr align_t, int first ){

	node_style *new_style = malloc( sizeof( node_style ) );
	memset( new_style, 0, sizeof( node_style ) );
	if( ( has_changed & 0x1 ) != 0 ){
		// face change, just copy node content
		new_style->font = malloc( strlen( face_t ) + 1 );
		memcpy( new_style->font, face_t, strlen( face_t ) +1 );
	}else{
		// same face (maybe NULL), if not NULL then copy content
		if( ((node_style*)(stack_head->data))->font != NULL ){
			new_style->font = malloc( strlen( ((node_style*)(stack_head->data))->font ) +1 );
			memcpy( new_style->font, ((node_style*)(stack_head->data))->font,
					strlen( ((node_style*)(stack_head->data))->font ) +1 );
		}
	}
	if( ( has_changed & 0x2 ) != 0 ){
		// size change
		new_style->size = malloc( strlen( size_t ) + 1 );
		memcpy( new_style->size, size_t, strlen( size_t ) +1 );
	}else{
		// same size (maybe NULL), if not NULL then copy content
		if( ((node_style*)(stack_head->data))->size != NULL ){
			new_style->size = malloc( strlen( ((node_style*)(stack_head->data))->size ) +1 );
			memcpy( new_style->size, ((node_style*)(stack_head->data))->size,
					strlen( ((node_style*)(stack_head->data))->size ) +1 );
		}
	}
	if( ( has_changed & 0x4 ) != 0 ){
		// color change
		new_style->color = malloc( strlen( color_t ) + 1 );
		memcpy( new_style->color, color_t, strlen( color_t ) +1 );
	}else{
		// same color (maybe NULL), if not NULL then copy content
		if( ((node_style*)(stack_head->data))->color != NULL ){
			new_style->color = malloc( strlen( ((node_style*)(stack_head->data))->color ) +1 );
			memcpy( new_style->color, ((node_style*)(stack_head->data))->color,
					strlen( ((node_style*)(stack_head->data))->color ) +1 );
		}
	}
	if( ( has_changed & 0x8 ) != 0 ){
		// align change
		new_style->align = malloc( strlen( align_t ) + 1 );
		memcpy( new_style->align, align_t, strlen( align_t ) +1 );
	}else{
		// same color (maybe NULL), if not NULL then copy content
		if( ((node_style*)(stack_head->data))->align != NULL ){
			new_style->align = malloc( strlen( ((node_style*)(stack_head->data))->align ) +1 );
			memcpy( new_style->align, ((node_style*)(stack_head->data))->align,
					strlen( ((node_style*)(stack_head->data))->align ) +1 );
		}
	}

	if( first != 0 ){
		// stack was empty
		// I created an "empty" style, so I have to delete it
		free( ((node_style*)(stack_head->data)) );
		// now I put the new style there
		stack_head->data = new_style;
	}else{
		// push into stack
		// I have to create a new node
		list_node *new_head = malloc( sizeof( list_node ) );
		new_head->data = new_style;
		new_head->next = stack_head;
		stack_head = new_head;
	}

#ifdef PRINT_LESS
	// print stack content
	list_node *stk_ptr = stack_head;
	//named_style *temp = malloc( sizeof( named_style ) );
	//memset( temp, 0, sizeof( named_style ) );
	printf("\nSTACK CONTENT\n");
	while( stk_ptr != NULL ){
		printf("\t%s, %s, %s, %s\n",
				((node_style*)(stk_ptr->data))->font,
				((node_style*)(stk_ptr->data))->color,
				((node_style*)(stk_ptr->data))->size,
				((node_style*)(stk_ptr->data))->align );
		// search in list
		//printf("...searching...\n");
		//temp->style = ((node_style*)(stk_ptr->data));
		//double_list_node* res = find_item( &list_head, temp, compare_named_style );
		//if( res != NULL ){
		//	printf("\tThis style is %s\n", ((named_style*)(res->data))->name );
		//}
		stk_ptr = stk_ptr->next;
	}
	//temp->name = NULL; temp->style = NULL;
	//free( temp );
	printf("\n");
#endif

}

void process_named_node( ctmbstr name, const char* indent ){

	/* if it has a name, then it's an HTML tag ... */
	//TidyAttr attr;
	// we have to get the name of current style!
	// get stack head, if NULL style is default
	node_style *current_style = ( ( stack_head != NULL ) ?
			((node_style*)stack_head->data) : NULL );
	// search in style list
	if( current_style != NULL ){
		// get default to fill blanks
		int restore_values = 0;
		named_style search_pattern;
		search_pattern.name = (char*)name;
		double_list_node* res = find_item( &list_head, &search_pattern, is_default_style );
		if( res != NULL ){
			restore_values = fill_style_with_default( current_style, ((named_style*)res->data)->style );
		}
		search_pattern.name = NULL;
		search_pattern.style = current_style;

		int exit_loop = 0;
		double_list_node *search_point = list_head;
		while( exit_loop == 0 ){
			// TODO this search for tag name and style can be done with a new search function
			res = find_item( &search_point, &search_pattern, compare_named_style );
			if( res != NULL ){
				// check if applies
				char *blank = strchr( ((named_style*)(res->data))->name, ' ' );
				int eq = -1;
				if( blank != NULL ){
					eq = strncmp( (char*)name, ((named_style*)(res->data))->name,
							( blank - ((named_style*)(res->data))->name ) );
				}else{
					eq = strcmp( (char*)name, ((named_style*)(res->data))->name );
				}
				if( eq == 0 ){
					exit_loop = 1;
					put_tag_in_output_file( ((named_style*)(res->data))->name, indent );
				}else{
					search_point = res->next;
				}
			}else{
				exit_loop = 1;
			}
		}
		if( res == NULL ){
			int new_len = strlen( (char*)name ) + 5 + 1; // "tag id00\0" = len(tag) + 4 + 1
			char *end_style = malloc( new_len );
			memset( end_style, '\0', new_len );
			sprintf( end_style, "%s id%0*d", name, 2, unknown_index );
			unknown_index++;
			// fill new list entry
			named_style *new_unknown = malloc( sizeof( named_style ) );
			new_unknown->name = end_style;
			new_unknown->style = copy_style_node( current_style );

			put_item( &list_head, new_unknown );
			put_tag_in_output_file( end_style, indent );
			// put style in list, for future reference
		}
		search_pattern.style = NULL;
		if( restore_values != 0 ){
			// values get back to NULL
			if( ( restore_values & 0x1 ) != 0 ) current_style->font = NULL;
			if( ( restore_values & 0x2 ) != 0 ) current_style->color = NULL;
			if( ( restore_values & 0x4 ) != 0 ) current_style->size = NULL;
			if( ( restore_values & 0x8 ) != 0 ) current_style->align = NULL;
		}
	}else{
		// el estilo no está definido -> default
		put_tag_in_output_file( name, indent );
	}
}

/*
void dump_style_list_as_css( double_list_node** list_head ){
	// iterate list using my custom function
	print_list( list_head, print_style_as_css );
}
*/
