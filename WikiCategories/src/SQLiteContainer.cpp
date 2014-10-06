/*
 * SQLiteContainer.cpp
 *
 *  Created on: Oct 4, 2014
 *      Author: pads
 */

#include "SQLiteContainer.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <vector>
#include "stdio.h"


// This is the callback function to display the select data in the table
int creation_callback(void *db_status, int argc, char **argv, char **szColName){
	int *return_val = (int*)db_status;
	if( argc == 1 ){
		// atoi from char*
		*return_val = atoi( argv[0] );
		/*
		// stringstream from string
		stringstream convert( std::string( argv[0] ) ); // stringstream used for the conversion initialized with the contents of Text

		if ( !(convert >> (*return_val)) )	//give the value to Result using the characters in the string
			*return_val = 0;	//if that fails set Result to 0
		*/
	}
	return 0;
}

int callback_select(void *vector, int argc, char **argv, char **szColName){
	std::vector<std::string> *return_val = (std::vector<std::string>*)vector;
	// argv = { cat_name, art_name }
	// szColname = { column1_name, column2_name
	//if( strcmp ( szColname[argc-1], "Article" ) == 0 )
	return_val->push_back( argv[argc-1] );
	return 0;
}

int callback(void *db_status, int argc, char **argv, char **szColName){
	std::cout << "Args: " << argc << "\n";
	for(int i = 0; i < argc; i++){
		std::cout << szColName[i] << " = " << argv[i] << std::endl;
	}
	std::cout << "\n";
	return 0;
}

SQLiteContainer::SQLiteContainer() {

	char *szErrMsg = 0;

	// prepare our sql statements
	std::cout << "Database creation\n";
	// open database
	int rc = sqlite3_open("wiki.db", &db);
	if(rc){
		std::cout << "Can't open database\n";
	} else {
		std::cout << "Open database successfully\n";
	}	// execute sql
	int value = 0;
	//const char *pSQL = "SELECT name FROM sqlite_master WHERE type='table' AND name='WikiCategories';";
	const char *pSQL = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='table_name';";
	rc = sqlite3_exec(db, pSQL, creation_callback, &value, &szErrMsg);
	std::cout << "Command executed... waiting\n";
	if(rc != SQLITE_OK){
		std::cout << "SQL Error: " << szErrMsg << std::endl;
		sqlite3_free(szErrMsg);
	}


	std::cout << "Value returned " << value << "\n";
	if( value == 0 ){
		// create table// prepare our sql statements
		const char *pSQL = "CREATE TABLE "
				"WikiCategories("
					"Categorie varchar(128), Article varchar(128))";// execute sql

		rc = sqlite3_exec(db, pSQL, callback, 0, &szErrMsg);
		if(rc != SQLITE_OK)
		{
			std::cout << "SQL Error: " << szErrMsg << std::endl;
			sqlite3_free(szErrMsg);
		}
	}
}

int SQLiteContainer::storeNewEntry( std::string *entry ){
	if( !db )
		return -1;
	char *szErrMsg = 0;
	// database is open
	int rc;

	// we have to parse the string
	std::vector<std::string> strs;
	boost::split(strs, *entry, boost::is_any_of(":"));
	std::string complete_sql = "INSERT INTO WikiCategories(Categorie, Article) VALUES ";
	std::string value_pairs = "";
	for( size_t i = 1; i < strs.size(); i++ ){
		/*sprintf( pSQL, "INSERT INTO WikiCategories(Categorie, Article) "
			"VALUES ('%s', '%s')", strs[i].c_str(), strs[0].c_str() );
			*/
		value_pairs.append( "('" + strs[i] + "','" + strs[0] + "')" +
				+ ( ( i == strs.size() - 1 ) ? ";" : "," ) );
	}
	complete_sql.append( value_pairs );
	std::cout << "SQL SENTENCE:\n\t" << complete_sql << "\n";
	rc = sqlite3_exec(db, (const char*)complete_sql.c_str(), callback, 0, &szErrMsg);
	if(rc != SQLITE_OK)
	{
		std::cout << "SQL Error: " << szErrMsg << std::endl;
		sqlite3_free(szErrMsg);
	}
	return 0;
}

void SQLiteContainer::print(){

	if( db ){

		char *szErrMsg = 0;

		// open database
		int rc = sqlite3_open("wiki.db", &db);

		if(rc)
		{
			std::cout << "Can't open database\n";
		} else {
			std::cout << "Open database successfully\n";
		}
		// prepare our sql statements
		const char *pSQL = "SELECT * FROM WikiCategories";

		// execute sql
		rc = sqlite3_exec(db, pSQL, callback, 0, &szErrMsg);
		std::cout << "Command executed... waiting\n";
		if(rc != SQLITE_OK)
		{
			std::cout << "SQL Error: " << szErrMsg << std::endl;
			sqlite3_free(szErrMsg);
		}
	}
}

std::vector< std::string >* SQLiteContainer::articlesInCategory( std::string cat ){

	if( !db )
		return NULL;
	char *szErrMsg = 0;
	std::vector< std::string >* retrieved = NULL;	// prepare our sql statements
	std::string sql = "SELECT * FROM WikiCategories WHERE Categorie='" + cat + "';";

	// execute sql
	retrieved = new std::vector<std::string>;
	int rc = sqlite3_exec(db, (const char*)sql.c_str(), callback_select, retrieved, &szErrMsg);
	std::cout << "Command executed... waiting\n";
	if(rc != SQLITE_OK)
	{
		std::cout << "SQL Error: " << szErrMsg << std::endl;
		sqlite3_free(szErrMsg);
	}

	return retrieved;
}

SQLiteContainer::~SQLiteContainer() {
	// TODO Auto-generated destructor stub
	// close database
	if(db)
	{
		sqlite3_close(db);
	}
}

