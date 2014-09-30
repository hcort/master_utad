/*
 * xmlparser.h
 *
 *  Created on: Sep 13, 2014
 *      Author: pads
 */

#ifndef XMLPARSER_H_
#define XMLPARSER_H_


#include <libxml/tree.h>
#include <libxml/parser.h>

#include "utils.h"

double_list_node* start_parsing_style( const char *filename );

void free_style_list( double_list_node **list );

#endif /* XMLPARSER_H_ */
