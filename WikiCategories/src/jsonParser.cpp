/*
 * jsonParser.cpp
 *
 *  Created on: Oct 1, 2014
 *      Author: pads
 */

#include "jsonParser.h"

#include "json_spirit_reader.h"
#include "json_spirit_value.h"
#include "json_spirit_writer.h"

#include <cassert>
#include <fstream>

#ifndef JSON_SPIRIT_VALUE_ENABLED
//#error Please define JSON_SPIRIT_VALUE_ENABLED for the Value type to be enabled
#endif

using namespace std;
using namespace json_spirit;


jsonParser::jsonParser() {
}

void print_type( Value_type type ){
	std::cout << "\t";
	if( type == obj_type )
		std::cout << "obj_type\n";
	else if( type ==  array_type )
		std::cout << "array_type\n";
	else if( type ==  str_type )
		std::cout << "str_type\n";
	else if( type == bool_type )
		std::cout << "bool_type\n";
	else if( type ==  int_type )
		std::cout << "int_type\n";
	else if( type ==  real_type )
		std::cout << "real_type\n";
	else if( type ==  null_type )
		std::cout << "null_type\n";
	else
		std::cout << "UNKNOWN\n";
}

std::string *find_categories( Value json_node ){
	Value_type type = json_node.type();
	//print_type( type );
	std::string title = "";
	if( type == obj_type ){
		//int size = json_node.size();
		Object obj = json_node.get_obj();
		for( Object::size_type i = 0; i != obj.size(); ++i )    {
			const Pair& pair = obj[i];
			const string& pname  = pair.name_;
			const Value&  pvalue = pair.value_;
			//std::cout << "\tName: " << pname << "\n";
			if( pname == "title" ){
				// grab article title
				if( pvalue.type( ) == str_type ){
					title = pvalue.get_str();
				}
			}

			std::string *found = find_categories( pvalue );
			if( found != NULL ){
				if( title != "" )
					found->insert( 0, title ); // inserting in the beginning
				return found;
			}
		}
	}else if( type == array_type ){
		Array arr = json_node.get_array();
		for( Object::size_type i = 0; i != arr.size(); ++i )    {
			std::string *found = find_categories( arr[i] );
			if( found != NULL ){
				return found;
			}
		}
	}else if( type == str_type ){
//		std::cout << "\tstring type:\t" << json_node.get_str() << "\n";
		std::string str = json_node.get_str();
		std::string needle = "[[Category";
		size_t len = str.length();
		if( len < needle.length( ) )
			return NULL;
		size_t pos_cat = (int)(len*0.8); /* we can assume categories are in the
											end of the string
										 */
		size_t pos_dots, pos_bracket;
		std::string *cat = NULL;
		while( ( pos_cat = str.find( needle, pos_cat ) ) != std::string::npos ){
			pos_dots = str.find(":", pos_cat );
			pos_bracket = str.find("]", pos_dots );
			if( ( pos_dots != std::string::npos ) && ( pos_bracket != std::string::npos ) ){
				if( cat == NULL ){
					cat = new std::string(
							str.substr( pos_dots, pos_bracket-pos_dots ) );
				}else{
					cat->append(
							str.substr( pos_dots, pos_bracket-pos_dots ) );
				}
			}
			pos_cat = pos_bracket;
		}
		return cat;
	}
	return NULL;
}

std::string* jsonParser::parse( std::string *json_object ){
	/*
	 *

    vector< Address > new_addrs = read_addrs( file_name );
	 */

	Value value;
	json_spirit::read( *json_object, value );
	//std::cout << "Root \n";
	//print_type( value.type() );
	//const Array& addr_array = value.get_array();
	std::string *cat = NULL;
	if( value.type() == obj_type )
		cat = find_categories( value.get_obj() );
	if( cat != NULL )
		std::cout << "Categories: " << *cat << "\n";
	else
		std::cout << "the cat was null\n";
	return cat;
}

jsonParser::~jsonParser() {
	// TODO Auto-generated destructor stub
}

