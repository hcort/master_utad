/*
 * FileAccess.h
 *
 *  Created on: Oct 2, 2014
 *      Author: pads
 */

#ifndef FILEACCESS_H_
#define FILEACCESS_H_

#include "NetworkAccess.h"
#include <fstream>

class FileAccess: public NetworkAccess {
public:
	FileAccess() {};

	std::string* getURL( std::string site, std::string page ){
		// file is in page.txt
		std::ifstream file;
		file.open( ( "cache/" + page + ".txt" ).c_str() );
		std::string *file_content = new std::string(
				(std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());
		file.close();
		return file_content;
	}
	virtual ~FileAccess() {};
};

#endif /* FILEACCESS_H_ */
