/******************************************************************************
Copyright (c) 2010 Casey Beach

File Name: mailsender.cc
Purpose: The mailsender class opens a socket and takes sends an email in the 
file pointed at by the string *filename

See COPYING for a copy of the mit licence
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <mailsender.hh>

using namespace std;

const int fail = -1;
const int succeed = 0;

//constructor
//all this really does is initialize and check the file stream
MailSender::MailSender(string &filename):EmailFile(filename.data())
{
	if(!EmailFile.is_open())
		throw("Invallid file");

}

//DOOM!
MailSender::~MailSender()
{
	close(SD);
	delete EmailFile;
}

//this function takes the host, to and from field, opens a socket to the host
//and sends the email in the file pointed at by EmailFile.
int 
MailSender::send(string &host_to, string &envelope_from, string &envelope_to)
{
	char *buf = new char[1024];	//temp char buffer
	int count = 0;	//usually used for getting the number of things
			//processed, like in the case of sendto's return
			//value

	OpenSocket(host_to);	//get an open socket


	string temp_string("");	//holds the messages to be sent to mailhost
	
	count = recv(SD,buf,1023,0);	//get the servers connection acknoledgement message
	buf[count] = '\0';


	//send a helo message
	cout << "HELO mailhost.cecs.pdx.edu" << endl;
	write(SD,"HELO snares.cs.pdx.edu\r\n",strlen("HELO snares.cs.pdx.edu\r\n"));
	count = recv(SD,buf,1023,0);	//get the responce
	buf[count] = '\0';
	
	if(!strstr(buf,"250 "))	//check for a 250 code for success
	{
		return fail;
	}
	
	
	//Send Mail From message
	temp_string.assign("MAIL FROM:");
	temp_string += envelope_from;
	temp_string += "\r\n";
	cout << temp_string << endl;
	
	write(SD, temp_string.c_str(), strlen(temp_string.c_str()));	//send to mailhost
	count = recv(SD,buf,1024,0);	//get responce
	buf[count] = '\0';
	
	if(!strstr(buf,"250 "))	//check for success
	{
		return fail;
	}
	

	//RCPT message
	temp_string.assign("RCPT TO: ");
	temp_string += envelope_to;
	temp_string += string("\r\n");
	cout << temp_string << endl;
	
	write(SD, temp_string.data(), strlen(temp_string.data()));	//send to mailserver
	count = recv(SD,buf,1024,0);
	buf[count] = '\0';

	if(!strstr(buf,"250 "))	//check for success
	{
		return fail;
	}

	//finally ready to send some data
	sendto(SD, "DATA\r\n", strlen("DATA\r\n"), 0, NULL, 0);
	count = recv(SD,buf,1024,0);	//get the servers responce
	cout << buf << endl;
	buf[count] = '\0';

	if(!strstr(buf,"354 "))	//checking for server conformation
	{
		return fail;
	}

	while(!EmailFile.eof())		//while there's still stuff in the
	{				//get a new line
		EmailFile.getline(buf,1024);	//get a line
		strcat(buf,"\r\n");		//put the <CRLF> on the end
		write(SD, (void*)(buf), strlen(buf));	//send it off
		cout << buf;		//tell the user

	}
	sendto(SD,"\r\n.\r\n",strlen("\r\n.\r\n"),0,NULL,0);	//end of data
	recv(SD,buf,1024,0);	//is the server happy?
	if(!strstr(buf,"250 "))	
		return -1;

	cout << buf << endl;	//it must be. WOOT!
	delete[] buf;

	return succeed;
}


//this opens a socket to your mailhost of choice
int 
MailSender::OpenSocket(string domain_name)
{
	hostent *MailServ;		//set up some structs
	sockaddr_in *MailAddress = new sockaddr_in;

	
	SD = socket(AF_INET, SOCK_STREAM, 0);	//open the socket
	if(SD < 0)	//check the socket
	{
		perror("unable to open socket");	
		exit(errno);
	}
	
	MailServ = gethostbyname(domain_name.data());	//get the host's IP
	if(MailServ == NULL)	//Fails!
	{
		perror("bad mailserver");
		exit(errno);
	}
	
	
	bzero(MailAddress, sizeof(*MailAddress));  //stuck in structure hell
	bcopy(	MailServ->h_addr,
		&MailAddress->sin_addr.s_addr,
		MailServ->h_length);	//made it out
	
	MailAddress->sin_family = AF_INET;	//set the protcol to TCP/IP
	MailAddress->sin_port = htons(25);	//port to 25
	if (connect(SD, 
		(sockaddr*)MailAddress, 
		sizeof(struct sockaddr)) != 
		0)//ENGAGE!
	{
		perror("could not connect");	//fail
		exit(errno);


	}


	cout << "socket created successfully!" << endl; //succeed!
	delete MailAddress;
	return succeed;
	
}
