/*
 * CurlAccess.h
 *
 *  Created on: Oct 4, 2014
 *      Author: pads
 */

#ifndef CURLACCESS_H_
#define CURLACCESS_H_

#include "NetworkAccess.h"

class CurlAccess: public NetworkAccess {
public:
	CurlAccess();
	std::string* getURL( std::string site, std::string page );
	~CurlAccess();
};

#endif /* CURLACCESS_H_ */
