#ifndef SVSSocket_TCP_h
#define SVSSocket_TCP_h 1

#include <string>

class SVSSocket_TCP
{
public:
	SVSSocket_TCP(const unsigned short port = 8888, const bool local_only = true);
	~SVSSocket_TCP();

	bool wait_for_connection();
	bool recieve_line(std::string &line);

private:
	
}

#endif
