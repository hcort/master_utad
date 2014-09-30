/*
 * fileoutput.c
 *
 *  Created on: Sep 29, 2014
 *      Author: pads
 */

#include "utils.h"
#include "style.h"
#include "list.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

char *temp_file = "/home/pads/workspace/TreeFormatter/temp_output.html";
char *output_file = "/home/pads/workspace/TreeFormatter/output.html";




void put_tag_in_output_file( const char* name, const char* indent ){
	if( name != NULL ){
		FILE *fp = fopen( temp_file, "a" ); // appending
		char *blank = strchr( name, ' ' );
		if( blank == NULL ) // default
			fprintf(fp, "<%s>\n", name );
		else{
			*blank = '\0';
			fprintf( fp, "%s<%s class=\"%s\">\n", indent, name, blank+1 );
			*blank = ' ';
		}
		fclose( fp );
	}
}

void put_text_in_ouput_file( const char* text, const char* indent ){
	FILE *fp = fopen( temp_file, "a" ); // appending
	fprintf(fp, "%s%s\n", indent, text );
	fclose( fp );
}

void close_tag_in_ouput_file( const char *tag, const char* indent ){
	FILE *fp = fopen( temp_file, "a" ); // appending
	fprintf(fp, "%s</%s>\n", indent, tag );
	fclose( fp );
}

void print_headers(){
	FILE *fp = fopen( output_file, "w" ); // appending
	fprintf(fp, " <!DOCTYPE html>\n<HEAD>\n" );
	// we left this open to put the css in here
	fclose( fp );
}

void print_style_as_css( const void *ns ){
	/*	From
	 * 		<p>font-family:arial;color:black;font-size:medium;text-align:left;</p>
			<p id="derecha">color:red;text-align:right;</p>
		To
			p { font-family:arial;color:black;font-size:medium;text-align:left; }
			p.derecha { font-family:arial;color:red;font-size:medium;text-align:right; }
	 */
	named_style *st = (named_style*)ns;
	if( ns != NULL ){
		FILE *fp = fopen( output_file, "a" ); // appending
		char *blank = strchr( st->name, ' ' );
		if( blank != NULL ) *blank = '.';
		fprintf(fp, "%s {\n\tfont-family:%s;\n\tcolor:%s;\n\tsize:%s;\n\ttext-align:%s;\n}\n", st->name,
			( st->style->font != NULL ) ? st->style->font : "" ,
			( st->style->color != NULL ) ? st->style->color : "" ,
			( st->style->size != NULL ) ? st->style->size : "" ,
			( st->style->align != NULL ) ? st->style->align : "" );
		//restore blank space
		if( blank != NULL ) *blank = ' ';
		fclose( fp );
	}
}

void dump_temp_into_output( double_list_node **list_head ){
	FILE *out = fopen( output_file, "w" ); // appending
	FILE *temp = fopen( temp_file, "r" ); // read only
	size_t buf_size = 256;
	char buffer[ buf_size + 1 ];
	memset( &buffer, '\0', buf_size+1 );
	size_t read_size = -1;
	int css_written = 0;
	char* head_end = NULL;
	while( ( read_size = fread( &buffer, sizeof(char) /* = 1*/, buf_size, temp) ) == buf_size ){
		// detect head tag
		if( css_written == 0 ){
			head_end = strstr( buffer, "</head>");
			if( head_end == NULL )
				head_end = strstr( buffer, "</HEAD>");

			if( head_end != NULL ){
				size_t chunk_size = ( head_end - buffer);
				fwrite( buffer, sizeof(char), chunk_size, out );
				fprintf( out, "<style type=\"text/css\">\n" );
				fclose( out );
				// iterate list using my custom function
				print_list( list_head, print_style_as_css );
				css_written = 1;
				out = fopen( output_file, "a" ); // appending
				fprintf( out, "</style>" );
				fwrite( head_end, sizeof(char), ( buf_size - chunk_size), out );
			}else{
				fwrite( buffer, sizeof(char), read_size, out );
			}
		}else{
			fwrite( buffer, sizeof(char), read_size, out );
		}
	}
	// write last chunk and close
	fwrite( buffer, sizeof(char), read_size, out );
	fclose( out );
	fclose( temp );

}
