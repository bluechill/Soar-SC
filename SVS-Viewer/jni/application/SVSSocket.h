//
//  SVSSocket.h
//  Application
//
//  Created by Alex Turner on 6/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Application_SVSSocket_h
#define Application_SVSSocket_h 1

#include "zenilib.h"

#include <string>
#include <vector>

//SVS Socket class, effectively the "server" (in a client server relationship).  It does not implement any send methods because as the viewer, it only views, it doesn't send data back to SVS only recieves from SVS.
class SVSSocket
{
public:
	SVSSocket(Uint16 port = 8888);
	~SVSSocket();

	bool wait_for_connection();
	bool recieve_line(std::string &line);

	bool client_disconnected();

private:
	Zeni::TCP_Listener listener;

	Zeni::TCP_Socket* client;
};

#endif
