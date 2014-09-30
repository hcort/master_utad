/*
 * pack.h
 *
 *  Created on: Sep 7, 2014
 *      Author: pads
 */

#ifndef PACK_H_
#define PACK_H_

#include "utils.h"

// this is the main entry point to the pack utilities
list_node* process_original_file( const char* filename );

int add_new_line( const char* buffer, int buffer_length );

list_node* start_packing( const char* filename );

void delete_list_from_packing();

#endif /* PACK_H_ */
