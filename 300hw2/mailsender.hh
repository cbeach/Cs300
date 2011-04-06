/******************************************************************************
Copyright (c) 2010 Casey Beach

File Name: mailsender.hh
Purpose: The mailsender class opens a socket and takes sends an email in the 
file pointed at by the string *filename

See COPYING for a copy of the mit licence
******************************************************************************/


#include <string>
#include <iostream>
#include <fstream>
#include <netdb.h>

using namespace std;

class MailSender {
	public:



		// Set up the class for sending the given file as email.
		// The file should contain an RFC-822 formatted email
		// message, whose correctness will be assumed by this object.

		MailSender(string &filename);	//constructor, initialize the file 
		~MailSender();

		// Actually send the email to the given host
		// with the given source and recipient.  Returns
		// 0 on success, and -1 on failure.  In the
		// case of failure, the global variable errno
		// will be set to indicate a failure code.

		virtual int send(string &host_to, string &envelope_from, string &envelope_to);

	protected:
		int SD, Port;
		ifstream EmailFile;
		//open a socke to the host given by domain_name;
		virtual int OpenSocket(string domain_name);



};
