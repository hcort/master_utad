/*
 * style.c
 *
 *  Created on: Sep 29, 2014
 *      Author: pads
 */

#include "style.h"

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

node_style* copy_style_node( const node_style* orig ){
	node_style *dest = malloc( sizeof( node_style ) );
	memset( dest, 0, sizeof( node_style ) );
	if( orig->font != NULL ){
		dest->font = malloc( strlen( orig->font ) + 1 );
		memset( dest->font, 0, strlen( orig->font ) + 1 );
		memcpy( dest->font, orig->font, strlen( orig->font ) );
	}
	if( orig->color != NULL ){
		dest->color = malloc( strlen( orig->color ) + 1 );
		memset( dest->color, 0, strlen( orig->color ) + 1 );
		memcpy( dest->color, orig->color, strlen( orig->color ) );
	}
	if( orig->size != NULL ){
		dest->size = malloc( strlen( orig->size ) + 1 );
		memset( dest->size, 0, strlen( orig->size ) + 1 );
		memcpy( dest->size, orig->size, strlen( orig->size ) );
	}
	if( orig->align != NULL ){
		dest->align = malloc( strlen( orig->align ) + 1 );
		memset( dest->align, 0, strlen( orig->align ) + 1 );
		memcpy( dest->align, orig->align, strlen( orig->align ) );
	}
	return dest;
}


named_style* create_named_style( char* name, char** attr_array ){
	named_style *new_style = malloc( sizeof( named_style ) );
	memset( new_style, 0, sizeof( named_style ) );
	new_style->style = malloc( sizeof( node_style ) );
	memset( new_style->style, 0, sizeof( node_style ) );
		/*
	struct node_style {
		char *font;		index = 0
		char *color;	index = 1
		char *size;		index = 2
		char *align;	index = 3
	};*/
	new_style->name = name;
	if( attr_array[0] != NULL ){
		new_style->style->font = malloc( strlen( attr_array[0] ) + 1 );
		memset( new_style->style->font, '\0', strlen( attr_array[0] ) + 1 );
		memcpy( new_style->style->font, attr_array[0], strlen( attr_array[0] ) );
	}
	if( attr_array[1] != NULL ){
		new_style->style->color = malloc( strlen( attr_array[1] ) + 1 );
		memset( new_style->style->color, '\0', strlen( attr_array[1] ) + 1 );
		memcpy( new_style->style->color, attr_array[1], strlen( attr_array[1] ) );
	}
	if( attr_array[2] != NULL ){
		new_style->style->size = malloc( strlen( attr_array[2] ) + 1 );
		memset( new_style->style->size, '\0', strlen( attr_array[2] ) + 1 );
		memcpy( new_style->style->size, attr_array[2], strlen( attr_array[2] ) );
	}
	if( attr_array[3] != NULL ){
		new_style->style->align = malloc( strlen( attr_array[3] ) + 1 );
		memset( new_style->style->align, '\0', strlen( attr_array[3] ) + 1 );
		memcpy( new_style->style->align, attr_array[3], strlen( attr_array[3] ) );
	}
	return new_style;
}

char* create_style_string( node_style* style ){
	if( style != NULL ){
		int len = ( ( style->font != NULL ) ? strlen(style->font) : 0 ) +
				( ( style->color != NULL ) ? strlen(style->color) : 0 ) +
				( ( style->size != NULL ) ? strlen(style->size) : 0 ) +
				( ( style->align != NULL ) ? strlen(style->align) : 0 );
		len = len + 26; // end of string and extra words
		char* str = NULL;
		if( len > 1 ){
			str = malloc( len );
			memset( str, '\0', len );
			sprintf ( str, "font:%s color:%s size:%s align:%s",
				( ( style->font != NULL ) ? style->font : "." ),
				( ( style->color != NULL ) ? style->color : "." ),
				( ( style->size != NULL ) ? style->size : "." ),
				( ( style->align != NULL ) ? style->align : "." ) );
			return str;
		}
	}
	return NULL;
}

/**
 * Only pointer handling
 */
int fill_style_with_default( node_style* custom, node_style* def ){
	int restore_values = 0;
	if( custom->font == NULL ){
		restore_values += 1;
		custom->font = def->font;
	}
	if( custom->color == NULL ){
		restore_values += 2;
		custom->color = def->color;
	}
	if( custom->size == NULL ){
		restore_values += 4;
		custom->size = def->size;
	}
	if( custom->align == NULL ){
		restore_values += 8;
		custom->align = def->align;
	}
	return restore_values;
}

/*
 * Uses memcpy to create copies
 */
void fill_values_with_default( named_style* to_fill, named_style* parent ){
	if( ( to_fill->style != NULL ) && ( parent->style != NULL ) ){
		node_style *dest = to_fill->style;
		node_style *orig = parent->style;
		if( ( dest->font == NULL ) && ( orig->font != NULL ) ){
			dest->font = malloc( strlen( orig->font ) + 1 );
			memset( dest->font, 0, strlen( orig->font ) + 1 );
			memcpy( dest->font, orig->font, strlen( orig->font ) );
		}
		if( ( dest->color == NULL ) && ( orig->color != NULL ) ){
			dest->color = malloc( strlen( orig->color ) + 1 );
			memset( dest->color, 0, strlen( orig->color ) + 1 );
			memcpy( dest->color, orig->color, strlen( orig->color ) );
		}
		if( ( dest->size == NULL ) && ( orig->size != NULL ) ){
			dest->size = malloc( strlen( orig->size ) + 1 );
			memset( dest->size, 0, strlen( orig->size ) + 1 );
			memcpy( dest->size, orig->size, strlen( orig->size ) );
		}
		if( ( dest->align == NULL ) && ( orig->align != NULL ) ){
			dest->align = malloc( strlen( orig->align ) + 1 );
			memset( dest->align, 0, strlen( orig->align ) + 1 );
			memcpy( dest->align, orig->font, strlen( orig->align ) );
		}
	}
}


/*
 * returns 0 if the names are "p" and "p basic", for example
 */
int compare_styles_by_name( const void* sa, const void* sb ){
	named_style *a = ( named_style* )sa;
	named_style *b = ( named_style* )sb;
	if( ( a->name == NULL ) && ( b->name == NULL ) )
		return 0;
	else if( ( ( a->name != NULL ) && ( b->name == NULL ) ) ||
			( ( a->name == NULL ) && ( b->name != NULL ) )){
		return 1;
	}else{
		char *first_space_a = strchr( a->name, ' ' );
		char *first_space_b = strchr( b->name, ' ' );
		// both are of the form "p basic" "p bold" and have space character
		if( ( first_space_a != NULL ) && ( first_space_b != NULL ) )
			return 1;
		int len_a = ( first_space_a == NULL ? 0 : first_space_a - a->name );
		int len_b = ( first_space_b == NULL ? 0 : first_space_b - b->name );
		int cmp = strncmp( a->name, b->name, ( len_a > len_b ? len_a : len_b ) );
		return cmp;
	}
}

void print_style( const void* data ){
	named_style *st = (named_style*)data;
	if( st->name != NULL )
		printf("Style: %s\n", st->name );
	else
		printf("Anon. style\n");
	if( st->style != NULL ){
		printf("\tFont: %s\n", ( ( st->style->font != NULL ) ? st->style->font : "." ) );
		printf("\tColor: %s\n", ( ( st->style->color != NULL ) ? st->style->color : "." ) );
		printf("\tSize: %s\n", ( ( st->style->size != NULL ) ? st->style->size : "." ) );
		printf("\tAlign: %s\n", ( ( st->style->align != NULL ) ? st->style->align : "." ) );
	}
}

void free_named_style( void* data ){
	named_style *st = (named_style*)data;
	if( st != NULL ){
		free( st->name );
		if( st->style != NULL ){
			if( st->style->font != NULL ) free( st->style->font );
			if( st->style->color != NULL ) free( st->style->color );
			if( st->style->size != NULL ) free( st->style->size );
			if( st->style->align != NULL ) free( st->style->align );
			free( st->style );
		}
		free( st );
	}

}


int is_default_style( const void* a, const void *b ){
	// looks for a style with the same name as "b" but without extra name
	const named_style* sa = (const named_style*)a;
	const named_style* sb = (const named_style*)b;
	if( ( sa == NULL ) && ( sb == NULL ) ){
		return 0;
	}else if( ( sa == NULL ) && ( sb != NULL ) ){
		return 1;
	}else if( ( sa != NULL ) && ( sb == NULL ) ){
		return 1;
	}else{
		if( ( sa->name != NULL ) && ( sb->name != NULL ) ){
			// compare two named styles?
			return strcmp( sa->name, sb->name );
		}
	}
	return 1;
}

/*
 * only detects equals or not-equals
 */
int compare_named_style( const void* a, const void *b ){
	const named_style* sa = (const named_style*)a;
	const named_style* sb = (const named_style*)b;
	if( ( sa == NULL ) && ( sb == NULL ) ){
		return 0;
	}else if( ( sa == NULL ) && ( sb != NULL ) ){
		return 1;
	}else if( ( sa != NULL ) && ( sb == NULL ) ){
		return 1;
	}else{
		if( ( sa->name != NULL ) && ( sb->name != NULL ) ){
			// compare two named styles?
			return strcmp( sa->name, sb->name );
		}else if( ( sa->style != NULL ) && ( sb->style != NULL ) ){
			// compare anonymous styles or one named, one anonymous
			int cmp = 0;
			if( ( ( sa->style->font == NULL ) && ( sb->style->font != NULL ) ) ||
					( ( sa->style->font != NULL ) && ( sb->style->font == NULL ) ) ){
				cmp++;
			}else if ( ( sa->style->font != NULL ) && ( sb->style->font != NULL ) ){
				if( strcmp( sa->style->font, sb->style->font ) != 0 )
					cmp++;
			}
			if( ( ( sa->style->color == NULL ) && ( sb->style->color != NULL ) ) ||
					( ( sa->style->color != NULL ) && ( sb->style->color == NULL ) ) ){
				cmp++;
			}else if ( ( sa->style->color != NULL ) && ( sb->style->color != NULL ) ){
				if( strcmp( sa->style->color, sb->style->color ) != 0 )
					cmp++;
			}
			if( ( ( sa->style->size == NULL ) && ( sb->style->size != NULL ) ) ||
					( ( sa->style->size != NULL ) && ( sb->style->size == NULL ) ) ){
				cmp++;
			}else if ( ( sa->style->size != NULL ) && ( sb->style->size != NULL ) ){
				if( strcmp( sa->style->size, sb->style->size ) != 0 )
					cmp++;
			}
			if( ( ( sa->style->align == NULL ) && ( sb->style->align != NULL ) ) ||
					( ( sa->style->align != NULL ) && ( sb->style->align == NULL ) ) ){
				cmp++;
			}else if ( ( sa->style->align != NULL ) && ( sb->style->align != NULL ) ){
				if( strcmp( sa->style->align, sb->style->align ) != 0 )
					cmp++;
			}
			return cmp;
		}else
			return 1;
	}
	return 0;
}


