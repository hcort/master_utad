/*
 * DataContainer.h
 *
 *  Created on: Oct 2, 2014
 *      Author: pads
 */

#ifndef DATACONTAINER_H_
#define DATACONTAINER_H_

#include <string>
#include <vector>
#pragma once

class DataContainer {
public:
	DataContainer() {};
	virtual int storeNewEntry( std::string *entry ) = 0 ;
	virtual std::vector< std::string >* articlesInCategory( std::string cat ) = 0;
	virtual void print() = 0;
	virtual ~DataContainer() {};
};

#endif /* DATACONTAINER_H_ */
