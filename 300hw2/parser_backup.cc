/******************************************************************************
Copyright (c) 2010 Casey Beach

File Name: Parser.cc
Purpose:   This class parses an email file.
******************************************************************************/


#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <parser.hh>
#include <ctype.h>

Parser::Parser(ifstream *email_descriptor)
{
	EmailFile = email_descriptor;

}

Parser::Parser(string *file_name)
{
	EmailFile = new ifstream(file_name->data());
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

Envelope* Parser::ParseMessage()
{
	Envelope *env = new Envelope();

	int to_or_from = 0;	//to: 1		from: 2

	char *line = new char[1024];				//single line from the email file
	char *multi_lines = new char[1024];			//multiple single lines cat'ed together
	char *token = new char[1024];				//stores tokens from the strtok function
	
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
			if(token[0] == '<')
			{
				string temp_string(token);
				temp_string = temp_string = temp_string.substr(1, temp_string.length() - 2);	

				switch(to_or_from)
				{
					case 1:
						env->To = new string(temp_string);
						while(strcmp(token = strtok(NULL," \n"),"from:")){}
						
						break;
					case 2:
						env->From = new string(temp_string);
						token = strtok(NULL, " \n");
						bool continue_while = true;
						
						while(continue_while == true)
						{
							if(token)
							{
								if(strcmp(token,"to:"))
								{
									while(token)
									{
										if(token[0] == '<')
										{
											cout << token << endl;

											temp_string.assign(token);
											temp_string = temp_string.substr(1, 
												temp_string.length() - 2);
											env->To = new string(temp_string);
											continue_while = false;
											break;
										}
										strtok(NULL," \n");
									}
								}
							}
						}

//						while(token = strtok(NULL," \n"), 

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
	
	

	cout << *env->From << endl;

	return env;
	

}



















