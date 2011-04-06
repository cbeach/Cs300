/******************************************************************************
Copyright (c) 2010 Casey Beach

File Name: 	main.cc
Purpose: 	Porgram entry point.

See COPYING for a copy of the mit licence
******************************************************************************/

#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <mailsender.hh>
#include <parser.hh>

using namespace std;

int main(int argc, char **argv)
{
	
	MailSender *client;
	Parser *parse;

	string host("mailhost.cecs.pdx.edu");	//this is my mailhost
						//there are many like it
						//but this one is mine...
	//check this cmd line input
	//and create some variables, making sure to catch thier
	//exceptions.
	if(argc == 2)
	{
		string temp_string(argv[1]);
		try
		{
			client = new MailSender(temp_string);
			parse = new Parser(temp_string);
		}
		catch(const char* error_string)
		{
			cout << error_string << ": exiting" << endl;
			exit(1);
		}

	}

	else
	{
		cout << "Usage: sender <email-file>" << endl;
		exit(-1);
	}

	Envelope *env;


	env = parse->ParseMessage();
	if(client->send(host, *env->From, *env->To) != 0)
		cout << "fatality!" << endl;
	delete env;
	delete client;
	delete parse;

}
