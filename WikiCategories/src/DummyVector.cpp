/*
 * DummyVector.cpp
 *
 *  Created on: Oct 3, 2014
 *      Author: pads
 */

#include "DummyVector.h"

void print_vector( std::vector<std::string> *v){
	std::cout << "Vector of size: " << v->size() << "\n";
	for( auto& x: *v ){
		std::cout << "\t" << x << "\n";
	}
}

DummyVector::DummyVector() {
	std::cout << "def const\n";
	this->content = new std::vector<std::string>;
}

DummyVector::DummyVector( const DummyVector &t ){
	std::cout << "copy constructor\n";
	this->content = t.content;
	//t.content = NULL;
}

DummyVector::DummyVector( const DummyVector &&t ){
	std::cout << "move constructor\n";
	this->content = t.content;
	//t.content = NULL;
}

DummyVector& DummyVector::operator=( const DummyVector &t ){
	std::cout << "copy assign operator = \n";
	this->content = t.content;
	return *this;
}

DummyVector& DummyVector::operator=( const DummyVector &&t ){
	std::cout << "move assign operator = \n";
	this->content = t.content;
	return *this;
}

DummyVector::~DummyVector() {
	std::cout << "def destr\n";
	this->content = NULL;
}

