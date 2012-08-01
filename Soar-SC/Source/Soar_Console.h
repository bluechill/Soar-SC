#ifndef SOAR_CONSOLE_H
#define SOAR_CONSOLE_H 1

#include <Windows.h>

#include "SDL/SDL.h"

#include "Events.h"

#include <vector>
#include <string>

#include <iostream>

class Soar_Console
{
public:
	Soar_Console(Events* event_queue);
	~Soar_Console();

	void send_input(std::string &input);
	void recieve_input(std::string &input);

private:
	Events* event_queue;

	SDL_Thread *console_thread;
};

#endif
