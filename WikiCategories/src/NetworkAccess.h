/*
 * NetworkAccess.h
 *
 *  Created on: Sep 30, 2014
 *      Author: pads
 */

#ifndef NETWORKACCESS_H_
#define NETWORKACCESS_H_
#pragma once

#include <string>

#define SAVE_CACHE

class NetworkAccess {
public:
	NetworkAccess() {};

	virtual std::string* getURL( std::string site, std::string page ) = 0;

	virtual ~NetworkAccess() {}
};

#endif /* NETWORKACCESS_H_ */
