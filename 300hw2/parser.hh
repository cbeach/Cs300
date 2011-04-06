/******************************************************************************
Copyright (c) 2010 Casey Beach

File Name: Parser.hh
Purpose:

******************************************************************************/
#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <string>

//struct string;
//struct ifstream;

using namespace std;

struct Envelope
{
	string *To;
	string *From;
	string *Data;
	string *EndToken;
};

class Parser
{
	public:
		Parser(ifstream &);
		Parser(string &file_name);
		~Parser();

		Envelope* ParseMessage();

	private:
		ifstream *EmailFile;

};

#endif
