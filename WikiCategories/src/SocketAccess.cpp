/*
 * SocketAccess.cpp
 *
 *  Created on: Sep 30, 2014
 *      Author: pads
 */

#include "SocketAccess.h"

#include<iostream>
#include <fstream>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <sys/socket.h>
#include <netdb.h> // gethostbyname
#include <resolv.h>
#include <arpa/inet.h>

SocketAccess::SocketAccess() {
	// TODO Auto-generated constructor stub
	std::cout << "Constructor of SocketAccess\n";

}

std::string* SocketAccess::getURL( std::string site, std::string page ){
	int s, error;
	struct sockaddr_in addr;

	if((s = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		std::cout <<"Error 01: creating socket failed!\n";
		//shutdown( s, SHUT_RDWR );
		close( s );
		return NULL;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);

	char *url = (char*)malloc( site.length() + page.length() + 4 );
	memset( url, '\0', site.length() + page.length() + 4 );

	sprintf( url, "%s%s:%d", site.c_str(), page.c_str(), 80 );

	struct hostent *h;

	std::cout << "Solving " << site << "\n";
	h = gethostbyname( site.c_str() );
	if( !h ){
		herror("SocketAccess DNS ERROR\t");
		std::cout << "Solving host: " << site << "\n";
		return NULL;
	}

	addr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)(h -> h_addr_list[0])));

	error = connect(s,(sockaddr*)&addr,sizeof(addr));
	if(error!=0)
	{
		std::cout<<"Error 02: conecting to server failed!\n";
		close(s);
		return NULL;
	}

	char msg[ 256 ] = {0};
	sprintf( msg, "GET /w/api.php?format=json&action=query&titles=%s&prop=revisions&rvprop=content HTTP/1.1\r\r"
			"Host: %s\r\n"
			"User-Agent: %s\r\n"
			"Cache-Control: no-cache\r\n\r\n", page.c_str(), site.c_str(), "tidmaster" );

	std::cout << msg << "\n";
	char answ[1024];
	//cin.getline(&msg[0],256);

	send(s,msg,sizeof(msg),0);

	bool json_found = false;
	std::string *json_answer = new std::string("");
	int bytes_rec = -1;
	while( ( bytes_rec = recv(s,answ,1024,0) ) !=0 ){
		if( bytes_rec != 1024 )
			answ[ bytes_rec ] = '\0';
		std::cout << answ << std::endl;
		// find first {
		if( !json_found ){
			char* opening = strchr( answ, '{' );
			if( opening != NULL ){
				json_found = true;
				json_answer->append( opening );
			}
		}else{
			json_answer->append( answ );
		}
	}

	std::cout << *json_answer << "\n";


	close(s);
	//std::cin.getline(&msg[0],1);
#ifdef SAVE_CACHE
	std::ofstream file;
	file.open( ( "cache/" + page + ".txt" ).c_str() );
	file << *json_answer << "\n";
	file.close();
#endif

	return json_answer;
}

SocketAccess::~SocketAccess() {
	// TODO Auto-generated destructor stub
	std::cout << "Constructor of SocketAccess\n";
}

