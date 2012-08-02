#ifndef EVENTS_H
#define EVENTS_H 1

#include <deque>

#include "sml_Client.h"

#include "Event.h"

#include "SDL/SDL.h"

class Soar_Console;

class Events
{
public:
	Events(Soar_Console* console);
	~Events();

	void update(bool lock = true);

	void add_event(Soar_Event event);

	void set_agent(sml::Agent* agent);

	bool update_forever();
	bool update_only_on_calls();

	SDL_cond* get_output_condition();
	SDL_cond* get_condition();
	SDL_mutex* get_mutex();
	
#ifdef EVENTS_PRIV
	void update_thread();
#endif

private:
	SDL_mutex* mu;
	SDL_cond* condition;
	SDL_cond* output_condition;

	bool interrupted;

	SDL_Thread* events_thread;

	bool should_die;

	sml::Agent* agent;

	std::deque<Soar_Event> event_queue;

	Soar_Console* console;
};

#ifdef EVENTS_PRIV
extern int events_global_thread(void* data);

typedef struct {
	sml::Agent* a;
	std::string command;
} Soar_Command;

extern int soar_command_thread(void* data);
#endif

#endif