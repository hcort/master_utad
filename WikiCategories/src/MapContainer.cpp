/*
 * MapContainer.cpp
 *
 *  Created on: Oct 2, 2014
 *      Author: pads
 */

#include "MapContainer.h"
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <vector>

#include "DummyVector.h"

MapContainer::MapContainer() {
	// nothing to do here
}

int MapContainer::storeNewEntry( std::string *entry ){
	// we have to parse the string
	std::vector<std::string> strs;
	boost::split(strs, *entry, boost::is_any_of(":"));
	for( size_t i = 1; i < strs.size(); i++ ){
		//std::cout << ( i != 0 ? "\t" : "Entry:" ) << strs[i] << "\n";
		//std::pair<std::string, std::string> map_entry { strs[0], strs[i] };
		map.insert( { strs[i], strs[0] } );
	}
	return 0;
}


#ifdef DUMMY_CLASS
struct store {
	// example of "invisible" copies
	DummyVector storage;
	void operator()(multimap::value_type& item){
		this->storage.put_item( item.second );
	}
};
#endif

struct store_ptr {
	std::vector < std::string > *storage;
	void operator()(multimap::value_type& item){
		this->storage->push_back( item.second );
	}
	/*  Done for show
	store_ptr(){
		this->storage = NULL;
	}
	store_ptr( const store_ptr &t ){
		this->storage = t.storage;
	}
	store_ptr( const store_ptr &&t ){
		this->storage = t.storage;
	}
	~store_ptr(){
		// avoid deleting data
		this->storage = NULL;
	}
	*/
};


std::vector< std::string >* MapContainer::articlesInCategory( std::string cat ){
	struct store_ptr result;
	result.storage = new std::vector<std::string>;
	std::cout << "Searching for: " << cat << "\n";
	auto range = this->map.equal_range( cat );
	for_each (
		range.first,
		range.second,
		result );
	return result.storage;
}

void MapContainer::print(){
	for( auto& x: this->map ){
		std::cout << "Cat: " << x.first << ", article: " << x.second << "\n";
	}
}

MapContainer::~MapContainer() {
	//this->map.
}

