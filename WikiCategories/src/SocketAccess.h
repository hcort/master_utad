/*
 * SocketAccess.h
 *
 *  Created on: Sep 30, 2014
 *      Author: pads
 */

#ifndef SOCKETACCESS_H_
#define SOCKETACCESS_H_

#include "NetworkAccess.h"

class SocketAccess: public NetworkAccess {
public:
	SocketAccess();
	std::string* getURL( std::string site, std::string page );
	virtual ~SocketAccess();
};

#endif /* SOCKETACCESS_H_ */
