//============================================================================
// Name        : WikiCategories.cpp
// Author      : Héctor
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "SocketAccess.h"
#include "CurlAccess.h"
#include "FileAccess.h"
#include "jsonParser.h"
#include "MapContainer.h"
#include "SQLiteContainer.h"

enum conn_type { socket_conn, boost_conn, curl_conn, file_conn };
enum data_type { database, boostmap };

int main( int argc, char *argv[]) {

	std::string article_list = argv[1];

	//
	conn_type conn = curl_conn;
	data_type data = database;
	NetworkAccess *net_conn = NULL;
	if( conn == socket_conn ){
		net_conn = new SocketAccess();
	}else if( conn == boost_conn ){

	}else if( conn == curl_conn ){
		net_conn = new CurlAccess();
	}else if( conn == file_conn ){
		net_conn = new FileAccess();
	}else{
		//error
		return -1;
	}
	std::string site = "en.wikipedia.org";
	std::ifstream input( article_list.c_str() );

	DataContainer *data_store = NULL;
	if( data == database ){
		data_store = new SQLiteContainer();
	}else if( data == boostmap ){
		data_store = new MapContainer();
	}
	for( std::string page; getline( input, page ); )
	{
		//...for each line in input...
		// should be done with pointers to avoid copies?
		std::string *response = net_conn->getURL( site, page );
		int store_code = 0;
		if( response != NULL ){
			//std::cout << "Parsing:\n\t" << page << "\n";
			jsonParser *p = new jsonParser();
			std::string *new_entry = p->parse( response );
			delete response;
			store_code = data_store->storeNewEntry( new_entry );
			if( store_code < 0 )
				std::cout << "Error inserting data\n";
			//std::cout << "\t" << new_entry << "\n";
		}else{
			std::cout << "Bad retrieval of " << page << "\n";
		}
	}

	//data_store->print();
	std::string cat = "Underground rappers";
	std::vector< std::string > *ret = data_store->articlesInCategory( cat );
	if( ret != NULL ){
		std::cout << "articles in category " << cat << ". " << ret->size( ) << " items\n";
		for( auto& x: *ret ){
			std::cout << x << "\n";
		}
		delete ret;
	}
	std::cout << "Exiting...\n";
	return 0;
}
