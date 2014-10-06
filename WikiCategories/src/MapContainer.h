/*
 * MapContainer.h
 *
 *  Created on: Oct 2, 2014
 *      Author: pads
 */

#ifndef MAPCONTAINER_H_
#define MAPCONTAINER_H_

#include "DataContainer.h"
#include <boost/unordered_map.hpp>

typedef boost::unordered_multimap< std::string, std::string > multimap;

class MapContainer: public DataContainer {
public:
	MapContainer();

	int storeNewEntry( std::string *entry );
	//#define DUMMY_CLASS
	#define POINTER_STRUCT
	std::vector< std::string >* articlesInCategory( std::string cat );

	void print();
	virtual ~MapContainer();
private:
	multimap map;
};

#endif /* MAPCONTAINER_H_ */
