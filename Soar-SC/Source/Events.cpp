#define EVENTS_PRIV 1

#include "Events.h"

#include "Soar_Console.h"
#include "Soar_Link.h"

#include <string>

using namespace std;

Events::Events(Soar_Console* console, Soar_Link* link)
{
	mu = SDL_CreateMutex();

	if (mu == NULL)
	{
		std::cerr << "Unable to create mutex for event thread, things will probably go bad.... exiting...." << std::endl;
		exit(7331);
	}

	condition = SDL_CreateCond();

	if (condition == NULL)
	{
		std::cerr << "Unable to create condition variable!" << std::endl;
		exit(7332);
	}

	output_condition = SDL_CreateCond();

	if (output_condition == NULL)
	{
		std::cerr << "Unable to create output_condition variable!" << std::endl;
		exit(7332);
	}

	should_die = false;

	this->console = console;
	this->link = link;

	interrupted = false;

	events_thread = SDL_CreateThread(events_global_thread, this);

	agent = NULL;
}

Events::~Events()
{
	should_die = true;

	SDL_WaitThread(events_thread, NULL);

	if (mu != NULL)
	{
		SDL_DestroyMutex(mu);
		mu = NULL;
	}
}

void Events::update(bool lock)
{
	std::cout << "UPDATE" << std::endl;
	if (lock)
		SDL_mutexP(mu);

	while (!event_queue.empty())
	{
		Soar_Event e = event_queue.front();
		event_queue.pop_front();

		switch (e.get_type())
		{
		case Soar_Event::SVS_Command:
			{
				agent->SendSVSInput(*e.get_command());
				break;
			}
		case Soar_Event::Console_Input:
			{
				if (e.get_command()->find("run") == 0)
				{
					Soar_Command* cmd = new Soar_Command;
					cmd->a = agent;
					cmd->command = *e.get_command();

					console->recieve_input(*e.get_command());

					SDL_CreateThread(soar_command_thread, cmd);
				}
				else
				{
					string output = agent->ExecuteCommandLine(e.get_command()->c_str());
					console->recieve_input(*e.get_command());
					console->recieve_input(output);
				}

				break;
			}
		case Soar_Event::WME_Destroy:
			{
				e.get_element()->DestroyWME();
				break;
			}
		case Soar_Event::New_Unit:
			{
				BWAPI::Unit* unit = e.get_unit();
				link->add_unit(unit);
				break;
			}
		default:
			break;
		}
	}

	if (lock)
		SDL_mutexV(mu);
}

void Events::add_event(Soar_Event event)
{
	std::cout << "ADD_EVENT" << std::endl;
	SDL_mutexP(mu);
	std::cout << "ADD_EVENT2" << std::endl;
	event_queue.push_back(event);
	std::cout << "ADD_EVENT3" << std::endl;
	SDL_mutexV(mu);
	std::cout << "ADD_EVENT4" << std::endl;
}

void Events::set_agent(sml::Agent* agent)
{
	std::cout << "SET_AGENT" << std::endl;
	this->agent = agent;
}

bool Events::update_forever()
{
	SDL_mutexP(mu);
	interrupted = true;
	SDL_mutexV(mu);
	SDL_CondSignal(condition);

	return true;
}

bool Events::update_only_on_calls()
{
	SDL_mutexP(mu);
	interrupted = false;
	SDL_mutexV(mu);

	return true;
}

void Events::update_thread()
{
	while (true)
	{
		SDL_mutexP(mu);
		while (!interrupted)
			SDL_CondWait(condition, mu);

		std::cout << "UPDATE_THREAD" << std::endl;

		if (should_die)
		{
			SDL_mutexV(mu);
			return;
		}

		update(false);
		
		SDL_mutexV(mu);

		SDL_CondSignal(output_condition);
	}
}

int events_global_thread(void* data)
{
	try {
		reinterpret_cast<Events*>(data)->update_thread();
	}
	catch(...) {
		return -1;
	}

	return 0;
}

int soar_command_thread(void* data)
{
	Soar_Command* cmd = reinterpret_cast<Soar_Command*>(data);

	cmd->a->ExecuteCommandLine(cmd->command.c_str());

	delete cmd;

	return 0;
}

SDL_mutex* Events::get_mutex()
{
	return mu;
}

SDL_cond* Events::get_output_condition()
{
	return output_condition;
}
