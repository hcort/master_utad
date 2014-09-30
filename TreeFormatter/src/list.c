/*
 * list.c
 *
 *  Created on: Sep 27, 2014
 *      Author: pads
 */


#include "list.h"
#include "stdlib.h"
#include "stdio.h"


/*
struct double_list_node {
	void *data;
	struct double_list_node *prev;
	struct double_list_node *next;
};
*/

/*
 *	from
 *		before -> insertion -> next
 *	to
 *		before->insertion->new->next
 */
double_list_node* put_item( double_list_node** insertion, void* data ){
	double_list_node *insertion_ptr = *insertion;
	double_list_node *point_ptr = NULL;
	point_ptr = malloc( sizeof( double_list_node ) );
	if( point_ptr == NULL )
		return NULL; // bad malloc
	point_ptr->prev = NULL;
	point_ptr->next = NULL;
	point_ptr->data = data;
	if( insertion_ptr == NULL ){
		// empty list
		*insertion = point_ptr;
	}else{
		// non-empty list
		point_ptr->prev = insertion_ptr;
		point_ptr->next = insertion_ptr->next;
		insertion_ptr->next = point_ptr;
		if( point_ptr->next != NULL ){
			point_ptr->next->prev = point_ptr;
		}
	}
	return point_ptr;
}

/*
 *	from
 *		before -> insertion -> next
 *	to
 *		before->next
 *	return data so you must handle memory
 */
void* delete_item( double_list_node **insertion){
	double_list_node *insertion_ptr = *insertion;
	if( insertion_ptr == NULL )
		return NULL;
	void *data = insertion_ptr->data;

	if( insertion_ptr->prev != NULL ){
		// not first
		insertion_ptr->prev->next = insertion_ptr->next;
	}
	if( insertion_ptr->next != NULL ){
		insertion_ptr->next->prev = insertion_ptr->prev;
	}
	free( insertion_ptr );
	*insertion = NULL;

	return data;
}

/*
 * insertion	where I start to search
 * data			the item I want to search
 * equals		a function to compare items
 */
double_list_node* find_item( double_list_node** insertion, void* data, int(*equals)(const void *, const void *) ){

	double_list_node *insertion_ptr = *insertion;
	while( insertion_ptr != NULL ){
		if( equals( data, insertion_ptr->data ) != 0 )
			insertion_ptr = insertion_ptr->next;
		else
			break;
	}
	return insertion_ptr;
}

int delete_list( double_list_node** insertion, void (*free_data)( void* )  ){
	double_list_node *insertion_ptr = *insertion;
	if( insertion_ptr == NULL )
		return 0;
	while( insertion_ptr != NULL ){
		// data memory handling delegated
		free_data( insertion_ptr->data );
		//free( insertion_ptr->data );
		double_list_node *temp = insertion_ptr;
		insertion_ptr = insertion_ptr->next;
		free( temp );
	}
	*insertion = NULL;
	return 0;
}
double_list_node* get_list_head( double_list_node** insertion ){
	if( insertion == NULL )
		return NULL;
	double_list_node *ptr = *insertion;
	while( ptr->prev != NULL )
		ptr = ptr->prev;
	return ptr;
}

void print_list( double_list_node** start, void (*print)( const void *data ) ){
	double_list_node* head = *start;
	if( head == NULL )
		printf("Empty list.\n");
	while( head != NULL ){
		print( head->data );
		head = head->next;
	}
}
