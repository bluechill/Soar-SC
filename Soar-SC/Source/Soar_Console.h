#ifndef SOAR_CONSOLE_H //Make sure this header is never included twice to prevent multiple includes
#define SOAR_CONSOLE_H 1

#include <Windows.h> //For windows functions used in creating the window

#include "SDL/SDL.h" //Include for SDL functions like SDL_CreateThread

#include "Events.h" //Include for event_queue class

#include <vector> //For std::vector
#include <string> //For std::string

#include <iostream> //For io functions and classes

class Soar_Console //Class for creating a seperate console window.  Instead of using the debugger, I use this custom console because it's a lot faster than the Soar debugger in a few circumstances.
{
public:
	Soar_Console(Events* event_queue); //Constructor, takes a pointer to an event_queue which will hold the console commands
	~Soar_Console(); //Deconstructor

	void send_input(std::string &input); //Send input to the agent, puts the commands in the event queue.
	void recieve_input(std::string &input); //Recieve input to put in the console log

private:
	Events* event_queue; //Internal event queue pointer

	SDL_Thread *console_thread; //Variable to hold the pointer to the SDL_Thread object of the console thread.  The console thread handles all the messages between all the objects

	int lines; //Number of lines used so far.  If it's over 1,000 it starts deleted one line for every one line added to prevent hitting the limit of the buffer.
};

#endif
