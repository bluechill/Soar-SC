#ifndef EVENTS_H //Make sure the header is only included once
#define EVENTS_H 1

#include <deque> //Header for queue

#include "sml_Client.h" //Header for Agent SML Client stuff (Soar stuff)

#include "Event.h" //Header for Event

#include "SDL/SDL.h" //Header for SDL stuff

class Soar_Console; //To prevent a circular include with Soar_Console.h only forward define it here
class Soar_Link; //To prevent a circular include with Soar_Link.h only forward define it here

class Events //A class for holding all the events (it's a queue class)
{
public:
	Events(Soar_Console* console, Soar_Link* link); //Constructor, takes a console pointer and a Soar_Link pointer
	~Events(); //Deconstructor

	void set_console(Soar_Console* console) { this->console = console; } //Set the console pointer

	void update(bool lock = true); //Update (loop through the queue and execute the commands), if lock is true then lock a mutex to prevent simultanous executions

	void add_event(Soar_Event event); //Add an event, handles mutexes to prevent simultanous access

	void set_agent(sml::Agent* agent); //Set the agent for the queue commands to be executed on
	
	bool update_forever(); //Tell the event queue thread to update forever (busy loop) or until
	bool update_only_on_calls(); //This function is called.  Only update when update() is called

	SDL_cond* get_condition(); //Get the internal condition variable for the thread.  Call SDL_CondSignal(...) to start the thread executing but this should be only called in update_forever().  This method isn't currently used
	SDL_mutex* get_mutex(); //Get the internal mutex.  Shouldn't need to use this
	
#ifdef EVENTS_PRIV //Private methods which only the Events class should have access to
	void update_thread(); //Function for the update thread, the function executed by the thread
#endif

private:
	SDL_mutex* mu; //Internal mutex to prevent simultanous access
	SDL_cond* condition; //Internal condition to start executing the thread (unsuspend it)

	bool interrupted; //Set to true to update forever so long as you call SDL_CondSignal(condition).  This is used by both update_forever() and update_only_on_calls()

	SDL_Thread* events_thread; //Thread variable

	bool should_die; //Set to true to have the thread die (if it is suspended make sure to call SDL_CondSignal(condition).)

	sml::Agent* agent; //Internal pointer to the agent, only used when the thread is told to update() or update_forever()

	std::deque<Soar_Event> event_queue; //Internal queue for the events

	Soar_Console* console; //Pointer for the console

	SDL_Thread* run_thread; //not used
	Soar_Link* link; //Pointer to the Soar_Link pointer
};

#ifdef EVENTS_PRIV //Private stuff only used by the Events class
extern int events_global_thread(void* data); //Event queue thread function

typedef struct { //Command structure used when calling soar_command_thread(void* data)
	sml::Agent* a;
	std::string command;
} Soar_Command;

extern int soar_command_thread(void* data); //Thread to execute soar commands in currently only used for "run" commands because they don't return until the run is stopped some how (halt, interrupt, stop-soar, etc.)
#endif

#endif