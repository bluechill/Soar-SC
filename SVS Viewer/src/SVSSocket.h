//
//  SVSSocket.h
//  Application
//
//  Created by Alex Turner on 6/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Application_SVSSocket_h
#define Application_SVSSocket_h

#include "zenilib.h"

#include <string>

#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#endif

//SVS Socket class, effectively the "server" (in a client server relationship).  It does not implement any send methods because as the viewer, it only views, it doesn't send data back to SVS only recieves from SVS.
class SVSSocket
{
public:
	typedef enum {
		SOCKET = 0,
		STDIN = 1,
		FILE_INPUT = 2
	} socket_type;
	
	SVSSocket(const char* path = ""); //Initializes SVSSocket with a path to the Unix Socket, however, on windows the path argument is ignored because on windows it uses pipes.  Ideally you should just leave this to the default which will be set if you leave it to nothing ie. SVSSocket();
	SVSSocket(const char* path = "");
	SVSSocket(bool standard_input); //Used for standard input
	SVSSocket(socket_type type);
	
	~SVSSocket(); //Dealloc and deconstruct everything
	
	bool listen(); //Accept a connection.  Warning, this is a *blocking* call which means it won't return until it has a connection
	bool recieve_line(std::string &line); //Read a line from the buffer.  Again, Warning, this is a *blocking* call which means it won't return until it has something to return
	
private:
	std::string recieve_buffer; //Buffer to handle input
	
#ifndef _WIN32 //If we're not on windows
	int listenfd, fd; //Use native unix domain sockets
	socklen_t len; //Length of socket
#else
	HANDLE pipe; //Since we're on windows, use pipes
#endif
	
	static const char* default_path_pipe; //Set in cpp file to the correct version.  On *nix this is a file path, on windows this is a pipe name.
	static const int buffer_size; //Buffer size, will read upto this amount, see cpp file for definition (10240)
	
	bool standard_input;

#ifdef _WIN32
	void RedirectIOToConsole();
#endif
	
	void initialize_stdin();
	void initialize_socket(const char* path);
	void initialize_file(const char* path);
	
	std::string hello_world;
};

#endif
