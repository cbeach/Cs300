/******************************************************************************
Copyright (c) 2010 Casey Beach

File Name: Parser.cc
Purpose:   This class parses an email file.

See COPYING for a copy of the mit licence
******************************************************************************/


#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <parser.hh>
#include <ctype.h>

Parser::Parser(ifstream &email_descriptor)
{
	EmailFile = &email_descriptor;

}

Parser::Parser(string &file_name)	//open the file and make
{					//sure it's good
	EmailFile = new ifstream(file_name.data());
	cout << file_name << endl;
	if(!EmailFile->is_open())
	{
		perror("unable to open email file, aborting program");
		exit(1);
	}
}

Parser::~Parser()
{
	EmailFile->close();
	delete EmailFile;
}

Envelope* 
Parser::ParseMessage()
{
	Envelope *env = new Envelope();

	int to_or_from = 0;	//to: 1		from: 2

	char *line = new char[1024];				//single line from the email file
	char *multi_lines = new char[1024];			//multiple single lines cat'ed together
	char *token = new char[1024];				//stores tokens from the strtok function
	multi_lines[0] = 0;
	
	for (int i = 0; i < 5; i++)				//this block of code gets the entire header of the email
	{							
		if ( !EmailFile->good())
			break;
		
		EmailFile->getline(line,1024);
		strcat(multi_lines, " ");
		strcat(multi_lines,line);
	}


	int i = 0;						//converts the entire string to lowercase
	while (multi_lines[i] != 0)
	{
		multi_lines[i] = tolower(multi_lines[i]);
		i++;
	}
	token = strtok(multi_lines, " \n");			//initialize the strtok function by getting the first token
	if ( !strcmp(token, "from:") || !strcmp(token, "to:"))	//check to see if the first token is the start of a to or from field
	{
		if (!strcmp(token, "from:"))
			to_or_from = 2;
		else if (!strcmp(token, "to:"))
			to_or_from = 1;
			
		while (token != NULL)
		{
			if(strchr(token,'@'))
			{
				string temp_string(token);

				switch(to_or_from)
				{
					case 1:
						env->To = new string(temp_string);
						to_or_from = 2;
						break;
					case 2:
						env->From = new string(temp_string);
						to_or_from = 1;
						break;
				}
			}

			
			
			token = strtok(NULL, " \n");				
		}
	}
	else 
	{
		throw("invalid email format");
	}
	

	env->Data = new string("data");
	delete[] line;
	delete[] multi_lines;
	delete[] token;
	return env;
	

}



















