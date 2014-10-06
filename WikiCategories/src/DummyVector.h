/*
 * DummyVector.h
 *
 *  Created on: Oct 3, 2014
 *      Author: pads
 */

#ifndef DUMMYVECTOR_H_
#define DUMMYVECTOR_H_

#include<string>
#include<iostream>
#include<vector>
#include <boost/unordered_map.hpp>

class DummyVector {
public:
	std::vector< std::string > *content;
	DummyVector();
	~DummyVector();
	DummyVector( const DummyVector &t );
	DummyVector( const DummyVector &&t );
	DummyVector& operator=( const DummyVector &t );
	DummyVector& operator=( const DummyVector &&t );
	void operator()(boost::unordered_multimap< std::string, std::string >::value_type& item){
		std::cout << "op() pushing " << item.second << "\n";
		this->content->push_back( item.second );
	}

	void put_item( std::string item ){
		std::cout << "inserting " << item << "...";
		this->content->push_back( item );
		std::cout << "\t(" << this->content->size() << " items)\n";
	}
};

#endif /* DUMMYVECTOR_H_ */
