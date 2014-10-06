/*
 * CurlAccess.cpp
 *
 *  Created on: Oct 4, 2014
 *      Author: pads
 */

#include "CurlAccess.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <curl/curl.h>

struct MemoryStruct {
	char *memory;
	size_t size;
};


static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

std::string* CurlAccess::getURL( std::string site, std::string page )
{
	CURL *curl;
	CURLcode res;


	struct MemoryStruct chunk;

	chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;    /* no data at this point */

	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	curl = curl_easy_init();
	std::string *json_answer = NULL;
	if(curl) {
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be a https:// URL if that is what should receive the
		   data. */
		char url[256];
		sprintf( url,
				"%s/w/api.php?format=json&action=query&titles=%s&prop=revisions&rvprop=content",
				site.c_str(), page.c_str() );


		curl_easy_setopt(curl, CURLOPT_URL, url );
		/* Now specify the POST data */
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, opt);


		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

		/* some servers don't like requests that are made without a user-agent
		 field, so we provide one */
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		else{
			//printf("%lu bytes retrieved\n", (long)chunk.size);
			//printf("\tCHUNK:%s\n", chunk.memory );
			json_answer = new std::string(chunk.memory);
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}

	if(chunk.memory)
		free(chunk.memory);
	curl_global_cleanup();
#ifdef SAVE_CACHE
	if( json_answer != NULL ){
		std::ofstream file;
		file.open( ( "cache/" + page + ".txt" ).c_str() );
		file << *json_answer << "\n";
		file.close();
	}
#endif
	return json_answer;
}

CurlAccess::CurlAccess() {
	// do something??
	std::cout << "Curl constr.\n";

}

CurlAccess::~CurlAccess() {
	std::cout << "Curl destroyer\n";
}

