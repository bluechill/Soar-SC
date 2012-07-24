#include <Windows.h>

#include "SDL/SDL.h"

#include <vector>
#include <string>

#include <iostream>

class Soar_Console
{
public:
	Soar_Console(std::vector<std::string> *console_queue, SDL_mutex* mu);
	~Soar_Console();

	void send_input(std::string &input);
	void recieve_input(std::string &input);

private:
	std::vector<std::string> *console_queue;
	SDL_mutex* mu;

	SDL_Thread *console_thread;
};