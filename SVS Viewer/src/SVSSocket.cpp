//
//  SVSSocket.cpp
//  Application
//
//  Created by Alex Turner on 6/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <zenilib.h>

#include "SVSSocket.h"

#include <iostream>
#include <fstream>

#include "SVSViewerState.h"

//If we're not on windows
#ifndef _WIN32
const char* SVSSocket::default_path_pipe = "/tmp/viewer"; //Set this to the default path, /tmp/viewer
#else
const char* SVSSocket::default_path_pipe = "\\\\.\\pipe\\svspipe"; //Otherwise set this to the pipe name
#endif

const int SVSSocket::buffer_size = 10240; //Set the buffer size to something reasonable

//SVSSocket constructor
SVSSocket::SVSSocket(const char *path)
{
	if (strlen(path) == 0) //If we have an empty path,
		path = default_path_pipe; //Set it to the default

	standard_input = false;

#ifndef _WIN32 //If we're not on windows
	struct sockaddr_un address; //Create a variable for our unix socket

	bzero((char *) &address, sizeof(address)); //Zero it out
	address.sun_family = AF_UNIX; //Set the socket to be a unix domain socket
	strcpy(address.sun_path, path);

	unlink(address.sun_path); //If we crashed earlier, make sure we don't have the viewer file around otherwise we will not be able to create a new socket at that path

	if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) //Create the socket and check for errors
	{
		perror("socket"); //We got an error, print it
		exit(1); //And exit the program
	}

	if (strlen(path) >= sizeof(address.sun_path)) //Make sure our path isn't bigger than the max path
	{
		perror("Path is too long."); //It is, so let's tell the user that
		exit(1); //And exit because we can't continue
	}

	strcpy(address.sun_path, path); //Set the path to it

#ifndef __APPLE__ //Mac OS X has a slightly different unix domain socket, so if we're on linux or some other *nix or *nix-like,
	socklen_t length = sizeof(address.sun_family) + strlen(address.sun_path) + 1;  //Create a variable with our socket size
	if (bind(listenfd, (struct sockaddr *) &address, length) == -1) //Then bind to the socket and start listening for connections
#else //But if we're on Mac OS X
	if (bind(listenfd, (struct sockaddr *) &address, SUN_LEN(&address)) == -1) //Bind but use the SUN_LEN function to get the length of the address
#endif
	{
		perror("bind"); //We had an error binding, print this to the user then
		exit(1); //Exit
	}

	if (::listen(listenfd, 1) == -1) //Make sure we can only have one connection, refuse any connections after the first
	{
		perror("socket"); //There was an error, tell the user about it
		exit(1); //Then exit
	}

	len = sizeof(struct sockaddr_un); //Set our length
#else //Windows Pipe Code
	static std::ofstream cerr_file("stderr.txt", std::ios::ate | std::ios::out | std::ios::app);
    static std::ofstream cout_file("stdout.txt", std::ios::ate | std::ios::out | std::ios::app);

    if(cerr_file.is_open()) {
      cerr_bak = std::cerr.rdbuf();
      std::cerr.rdbuf(cerr_file.rdbuf());
    }

    if(cout_file.is_open()) {
      cout_bak = std::cout.rdbuf();
      std::cout.rdbuf(cout_file.rdbuf());
    }
	
	pipe = CreateNamedPipe(path, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, buffer_size, buffer_size, 0, NULL); //Create a message pipe

	if (pipe == INVALID_HANDLE_VALUE) //Make sure it's valid
	{
		std::cout << "Error creating pipe: " << GetLastError() << std::endl; //Not valid so output it
		exit(1); //Then exit
	}
#endif
}

//Constructor for stdin
SVSSocket::SVSSocket(bool standard_input)
{
	if (!standard_input) //If we don't want to use stdin
	{
		::SVSSocket(); //Use our default constructor
		standard_input = false;
	}
	else //Otherwise
	{
		standard_input = true; //Set that we are using stdin

#ifdef _WIN32
		RedirectIOToConsole();
#endif
	}
}

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

static const WORD MAX_CONSOLE_LINES = 500;

void SVSSocket::RedirectIOToConsole()
{
	using namespace std;

	int hConHandle;

	long lStdHandle;

	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	FILE *fp;

	// allocate a console for this app

	AllocConsole();

	// set the screen buffer to be big enough to let us scroll text

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),

		&coninfo);

	coninfo.dwSize.Y = MAX_CONSOLE_LINES;

	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),

		coninfo.dwSize);

	// redirect unbuffered STDOUT to the console

	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stdout = *fp;

	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console

	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "r" );

	*stdin = *fp;

	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console

	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stderr = *fp;

	setvbuf( stderr, NULL, _IONBF, 0 );

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog

	// point to console as well

	ios::sync_with_stdio();
}

#endif

//Deconstructor
SVSSocket::~SVSSocket()
{
	if (standard_input) //Did we use stdin or a real socket?
		return; //We used stdin so no need to close any sockets or pipes

#ifndef _WIN32 //If we're not on windows
	close(fd); //Use te close function to close our socket
#else
	CloseHandle(pipe); //Otherwise close the pipe on windows
#endif
}

//Blocking "wait for a connection"
bool SVSSocket::listen()
{
	if (standard_input) //Are we using stdin or an actual socket?
		return true; //We're using stdin so no need to listen() so return true

#ifndef _WIN32 //If we're not on windows
	struct sockaddr_in remote; //Create a variable for the remote connection
	if ((fd = ::accept(listenfd, (struct sockaddr *) &remote, &len)) == -1) //Then accept the connection
	{
		perror("socket"); //There was an error if we reached here, tell the user
		return false; //Return that we had an error
	}
#else //On windows
	BOOL success = ConnectNamedPipe(pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); //Connect to the pipe
	if (!success) //But if we had an error
	{
		std::cout << "Error waiting for connection pipe: " << GetLastError() << std::endl;
		CloseHandle(pipe); //Close the pipe
		return false; //Then return that we had an error
	}
#endif

	return true; //Otherwise return success
}

#ifdef _WIN32
#include <conio.h>
#endif

int SVSSocket::inputAvailible()
{
#ifdef _WIN32
	return _kbhit();
#else
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	return (FD_ISSET(0, &fds));
#endif
}

//Recieve a line from the socket/pipe
bool SVSSocket::recieve_line(std::string &line)
{
	if (standard_input) //Are we using stdin or an actual socket?
	{
		while (!inputAvailible());

		getline(std::cin, line); //Get one line

		//We have no use for recieve_buffer here since we're using getline and only get one line at a time.  getline handles the rest for us along with cin.

		return true; //Return true that we've recieved one line
	}

	char buffer[buffer_size+1]; //Create a variable to store the recieved contents into

	bool should_quit = false;
	while(!should_quit) //Loop infinitely until we're done recieving
	{
		size_t i = recieve_buffer.find('\n'); //Create a variable for the size of the line not including the termination

#ifndef _WIN32 //If we're not on windows
		size_t n; //Create a size_t variable to store the number of characters recieved
#else //But if we're on windows
		DWORD n; //Create a DWORD to do that
#endif

		if (i != std::string::npos) //If it's not an invalid position
		{
			line = recieve_buffer.substr(0, i); //Create a substring up to that point
			recieve_buffer = recieve_buffer.substr(i + 1); //Then get rid of what we just substring'd
			return true; //Return true that we've recieved one line
		}

#ifndef _WIN32 //If we're not on windows
		if ((n = recv(fd, buffer, buffer_size, 0)) <= 0) //Recieve characters from the socket and put them into the buffer.  Then check if we had an error in doing such
			return false; //We did so return that we did not successfully recieve one line
#else //If we're on windows
		BOOL success = ReadFile(pipe, buffer, buffer_size*sizeof(char), &n, NULL); //Read the pipe's contents into the buffer

		if (!success || n == 0) //Check for failure or zero characters read
			return false; //If we had either one, return that we did not recieve a line
#endif

		buffer[n] = '\0'; //Make sure our buffer is null terminated
		recieve_buffer += buffer; //Add the recieved buffer to our storing string
	}

	return true;
}

void SVSSocket::reopen_pipe()
{
	DisconnectNamedPipe(pipe);
	CloseHandle(pipe);

	pipe = CreateNamedPipe(default_path_pipe, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, buffer_size, buffer_size, 0, NULL); //Create a message pipe

	if (pipe == INVALID_HANDLE_VALUE) //Make sure it's valid
	{
		std::cout << "Error creating pipe: " << GetLastError() << std::endl; //Not valid so output it
		exit(1); //Then exit
	}
}
