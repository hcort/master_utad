/*
 * unpack.h
 *
 *  Created on: Sep 7, 2014
 *      Author: pads
 */

#ifndef UNPACK_H_
#define UNPACK_H_


#include "utils.h"

list_node* read_packed_file( const char* filename );

void link_to_external_list( list_node* list_head );

list_node*  process_packed_strings( const char* filename );

#endif /* UNPACK_H_ */
