/*
 * SQLiteContainer.h
 *
 *  Created on: Oct 4, 2014
 *      Author: pads
 */

#ifndef SQLITECONTAINER_H_
#define SQLITECONTAINER_H_

#include "DataContainer.h"

#include "sqlite3.h"

class SQLiteContainer: public DataContainer {
public:
	SQLiteContainer();
	virtual int storeNewEntry( std::string *entry );
	std::vector< std::string >* articlesInCategory( std::string cat );
	void print();
	virtual ~SQLiteContainer();
private:
	sqlite3 *db;
};

#endif /* SQLITECONTAINER_H_ */
