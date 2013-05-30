//
//  SVSSocket.cpp
//  Application
//
//  Created by Alex Turner on 6/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <zenilib.h>

#include "SVSSocket.h"

SVSSocket::SVSSocket(Uint16 port)
: listener(port)
{
	client = nullptr;
}

SVSSocket::~SVSSocket()
{
	if (client)
		delete client;
}

bool SVSSocket::wait_for_connection()
{
	if (client)
		delete client;

	TCPsocket sock = nullptr;
	while (sock == nullptr)
	{
		sock = listener.accept();
		Sleep(1);
	}

	client = new Zeni::TCP_Socket(sock);

	return true;
}

bool SVSSocket::recieve_line(std::string &line)
{
	static Zeni::String data;

	if (client == nullptr)
		return false;

	Zeni::String buffer;
	while (data.find('\n', 0) == Zeni::String::npos)
	{
		buffer = "";
		
		try
		{
			client->receive(buffer, 10240); //Receive 512 bytes at a time
		}
		catch (Zeni::Socket_Closed e)
		{
			return false;
		}

		data += buffer;
	}

	Zeni::String recieved_line = data.substr(0, data.find('\n', 0));
	data.erase(0, data.find('\n', 0)+1);

	line = recieved_line.c_str();

	return true;
}

bool SVSSocket::client_disconnected()
{
	if (client == nullptr)
		return true;

	if (client->try_check_socket() == -1)
		return true;
	else
		return false;
}
