/*
 * list.h
 *
 *  Created on: Sep 27, 2014
 *      Author: pads
 */

#ifndef LIST_H_
#define LIST_H_

#include "utils.h"

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
double_list_node* put_item( double_list_node** insertion, void* data );

/*
 *	from
 *		before -> insertion -> next
 *	to
 *		before->next
 */
void* delete_item( double_list_node **insertion );

/*
 * start	where I start to search
 * data		the item I want to search
 * equals	a function to compare items
 */
double_list_node* find_item( double_list_node** start, void* data, int(*equals)(const void *, const void *) );

int delete_list( double_list_node** start, void (*free_data)( void* ) );

void print_list( double_list_node** start, void (*print)( const void * ) );

double_list_node* get_list_head( double_list_node** insertion );


#endif /* LIST_H_ */
