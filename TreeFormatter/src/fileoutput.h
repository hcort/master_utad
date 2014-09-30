/*
 * fileoutput.h
 *
 *  Created on: Sep 29, 2014
 *      Author: pads
 */

#ifndef FILEOUTPUT_H_
#define FILEOUTPUT_H_


extern char* temp_file;
extern char* output_file;

void print_style_as_css( const void *ns );

void dump_temp_into_output( double_list_node **list_head );

void put_tag_in_output_file( const char* name, const char* indent );

void put_text_in_ouput_file( const char* text, const char* indent );

void close_tag_in_ouput_file( const char *tag, const char* indent );

#endif /* FILEOUTPUT_H_ */
