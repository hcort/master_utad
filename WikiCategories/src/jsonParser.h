/*
 * jsonParser.h
 *
 *  Created on: Oct 1, 2014
 *      Author: pads
 */

#ifndef JSONPARSER_H_
#define JSONPARSER_H_

#include <string>

class jsonParser {
public:
	jsonParser();
	virtual ~jsonParser();

	std::string* parse( std::string *json_object );
};

#endif /* JSONPARSER_H_ */
