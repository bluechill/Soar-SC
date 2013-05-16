#include "Soar_SC.h"

#include "Soar_Link.h"
#include "BWAPI_Link.h"

using namespace std;
using namespace BWAPI;

//Typedefs Private Class stuff
typedef struct { //Command structure used when calling soar_command_thread(void* data)
	Soar_Link* soar_link;
	std::string command;
} Soar_Command;

extern int soar_sc_soar_command_thread(void* data);

extern int soar_sc_soar_event_thread(void* data);
extern int soar_sc_bwapi_event_thread(void* data);

//Class Functions

Soar_SC::Soar_SC()
	:	cout_redirect("bwapi-data/logs/stdout.txt"), //Initialize the replacement cout buffer with the path of the log file
	cerr_redirect("bwapi-data/logs/stderr.txt")
{
	cout_orig_buffer = cout.rdbuf(); //Set the original cout buffer to the backup one
	cerr_orig_buffer = cerr.rdbuf(); //Set the original cerr buffer to the backup one

	if (!cout_redirect || !cerr_redirect) //If either of the buffers isn't open then
		Broodwar->printf("Unable to redirect output!"); //Tell the user through Starcraft that the buffer isn't open and we couldn't redirect output
	else //Otherwise
	{
		cout.rdbuf(cout_redirect.rdbuf()); //redirect cout to the new cout buffer
		cerr.rdbuf(cerr_redirect.rdbuf()); //and redirect the cerr buffer to new cerr buffer
	}

	soar_event_mutex = SDL_CreateMutex();
	bwapi_event_mutex = SDL_CreateMutex();

	if (!soar_event_mutex || !bwapi_event_mutex)
	{
		cerr << "Unable to create either the Soar mutex or the BWAPI mutex! Will exit!" << endl;
		exit (1);
	}

	soar_thread_condition = SDL_CreateCond();
	bwapi_thread_condition = SDL_CreateCond();

	if (!soar_thread_condition || !bwapi_thread_condition)
	{
		cerr << "Unable to create either the Soar condition or the BWAPI condition! Will exit!" << endl;
		exit (2);
	}

	kill_threads = false;
	run_forever = false;

	SDL_CreateThread(soar_sc_soar_event_thread, this);
	SDL_CreateThread(soar_sc_bwapi_event_thread, this);

	soar_link = new Soar_Link(this);
	bwapi_link = new BWAPI_Link(this);
}

Soar_SC::~Soar_SC()
{
	delete soar_link;
	delete bwapi_link;

	kill_threads = true;

	SDL_CondSignal(soar_thread_condition);
	SDL_CondSignal(bwapi_thread_condition);

	SDL_WaitThread(soar_event_thread, nullptr); //Wait until the thread has executed
	SDL_WaitThread(bwapi_event_thread, nullptr); //Wait until the thread has executed

	if (soar_event_mutex) //Then make sure the mutex isn't NULL
	{
		SDL_DestroyMutex(soar_event_mutex); //And since it isn't, destroy the mutex
		soar_event_mutex = nullptr; //And set it to NULL
	}

	if (bwapi_event_mutex)
	{
		SDL_DestroyMutex(bwapi_event_mutex);
		bwapi_event_mutex = nullptr;
	}

	cout.rdbuf(cout_orig_buffer); //Redirect the cout buffer back to prevent errors after deallocation
	cerr.rdbuf(cerr_orig_buffer); //Do the same for the cerr buffer
}

void Soar_SC::add_event(Soar_Event event)
{
	SDL_mutexP(soar_event_mutex); //Lock the mutex to prevent simultanous access
	soar_event_queue.push(event); //Add the event to the internal queue
	SDL_mutexV(soar_event_mutex); //Then unlock the mutex
}

void Soar_SC::add_event(BWAPI_Event event)
{
	SDL_mutexP(bwapi_event_mutex);
	if (bwapi_event_queue.find(event.get_unit()) == bwapi_event_queue.end())
	{
		bwapi_event_queue[event.get_unit()] = new BWAPI_Event_Struct;
		bwapi_event_queue[event.get_unit()]->count = 0;
	}

	bwapi_event_queue[event.get_unit()]->queue.push(event);
	SDL_mutexV(bwapi_event_mutex);
}

void Soar_SC::signal_soar_updates()
{
	SDL_CondSignal(soar_thread_condition);

	if (!run_forever)
	{
		SDL_CondWait(soar_thread_condition, soar_event_mutex);
		SDL_mutexV(soar_event_mutex);
	}
}

void Soar_SC::signal_bwapi_updates()
{
	SDL_CondSignal(bwapi_thread_condition);

	if (!run_forever)
	{
		SDL_CondWait(bwapi_thread_condition, bwapi_event_mutex);
		SDL_mutexV(bwapi_event_mutex);
	}
}

void Soar_SC::set_should_run_forever(bool forever)
{
	SDL_mutexP(soar_event_mutex);
	SDL_mutexP(bwapi_event_mutex);

	run_forever = forever;

	SDL_mutexV(soar_event_mutex);
	SDL_mutexV(bwapi_event_mutex);
}

void Soar_SC::soar_thread_update(bool direct_call)
{
	SetThreadName("Soar Event Queue Thread", GetCurrentThreadId());

	while (true)
	{
		SDL_mutexP(soar_event_mutex);

		if (!run_forever && !direct_call)
			SDL_CondWait(soar_thread_condition, soar_event_mutex);

		if (kill_threads)
		{
			SDL_mutexV(soar_event_mutex);
			return;
		}

		while (!soar_event_queue.empty()) //While our queue isn't empty
		{
			Soar_Event event = soar_event_queue.front(); //Get the first event
			soar_event_queue.pop(); //Then remove it from the queue

			switch (event.get_type()) //Switch based on the type of the event
			{
			case Soar_Event::SVS_Command: //If it's a SVS Command
				{
					soar_link->SendSVSInput(*event.get_command()); //Send it to the agent

					break; //Then break out of the switch statement
				}
			case Soar_Event::Console_Input: //If it's Console Input
				{
					if (event.get_command()->find("run") == 0 || event.get_command()->find("time") == 0) //Check if it's a run command or a time command
					{
						Soar_Command* cmd = new Soar_Command; //Since it is, create a new Soar_Command struct for the thread
						cmd->soar_link = soar_link; //Set the agent for the thread
						cmd->command = *event.get_command(); //Set the command for the thread

						SDL_CreateThread(soar_sc_soar_command_thread, cmd); //Create a thread to execute the run command.  A run command never returns until the run stops.  This means that unless it's called from a seperate thread we will never be able to get info about the agent or stop the agent until it stops on it's own.
					}
					else //Otherwise
						soar_link->ExecuteCommandLine(event.get_command()->c_str()); //Execute it now

					break; //Then break
				}
			case Soar_Event::WME_Destroy: //If it's a destroy command
				{
					event.get_element()->DestroyWME(); //Destroy the WMElement

					break; //Then break
				}
			case Soar_Event::Status_Update:
				{
					sml::Identifier* output_link = soar_link->GetOutputLink();

					sml::Identifier::ChildrenIter it = find(output_link->GetChildrenBegin(), output_link->GetChildrenEnd(), event.get_identifier());

					if (it == output_link->GetChildrenEnd())
					{
						cout << "Coult not find Events Identifer: " << event.get_identifier() << " on the output link! ERROR! THIS WILL FAIL" << endl;
						break;
					}

					if (event.get_status())
						event.get_identifier()->AddStatusComplete();
					else
						event.get_identifier()->AddStatusError();

					break;
				}
			case Soar_Event::New_Unit: //If we're supposed to add a unit
				{
					BWAPI::Unit* unit = event.get_unit(); //Get the unit to add
					bwapi_link->add_unit(unit, unit->getPlayer()->isEnemy(BWAPI::Broodwar->self())); //Then add it

					break; //Then break
				}
			default: //If it's a default, handle it as an error.  This is never possible unless there is memory corruption in which case more things than just this will go wrong.
				{
					cerr << "Possible memory corruption! Event Type of '" << event.get_type() << "'" << endl; //Tell the user
					break; //Then break
				}
			}
		}

		SDL_mutexV(soar_event_mutex);

		if (!run_forever && !direct_call)
		{
			Sleep(10);
			SDL_CondSignal(soar_thread_condition);
		}

		if (direct_call)
			break;
	}
}

void Soar_SC::bwapi_thread_update(bool direct_call)
{
	SetThreadName("BWAPI Event Queue Thread", GetCurrentThreadId());

	while (true)
	{
		SDL_mutexP(bwapi_event_mutex);

		if (!run_forever && !direct_call)
			SDL_CondWait(bwapi_thread_condition, bwapi_event_mutex);

		if (kill_threads)
		{
			SDL_mutexV(bwapi_event_mutex);
			return;
		}

		std::vector<BWAPI::Unit*> to_erase;

		for (std::map<BWAPI::Unit*, BWAPI_Event_Struct* >::iterator it = bwapi_event_queue.begin();it != bwapi_event_queue.end();it++)
		{
			BWAPI_Event_Struct* event_struct = it->second;
			if (event_struct->count <= 0)
			{
				BWAPI_Event event = event_struct->queue.front();
				event_struct->queue.pop();

				event.execute_command();

				event_struct->count = 2;

				if (event_struct->queue.size() == 0)
					to_erase.push_back(it->first);
			}
			else
				event_struct->count--;
		}

		for (std::vector<BWAPI::Unit*>::iterator it = to_erase.begin();it != to_erase.end();it++)
			bwapi_event_queue.erase(bwapi_event_queue.find(*it));

		SDL_mutexV(bwapi_event_mutex);

		if (!run_forever && !direct_call)
		{
			Sleep(10);
			SDL_CondSignal(bwapi_thread_condition);
		}

		if (direct_call)
			break;
	}
}

int soar_sc_soar_event_thread(void* data) //Global function called by SDL.  The void* is a Soar_Link this pointer.
{
	try { //Try executing the event queue
		reinterpret_cast<Soar_SC*>(data)->soar_thread_update(false); //Convert the void* to a Soar_Link pointer, then call update_thread() to start doing the job the thread is supposed to, handle events
	}
	catch(...) { //Catch any errors
		return -1; //We had one so return -1
	}

	return 0; //We had no errors, so return 0, success
}

int soar_sc_bwapi_event_thread(void* data) //Global function called by SDL.  The void* is a Soar_Link this pointer.
{
	try { //Try executing the event queue
		reinterpret_cast<Soar_SC*>(data)->bwapi_thread_update(false); //Convert the void* to a Soar_Link pointer, then call update_thread() to start doing the job the thread is supposed to, handle events
	}
	catch(...) { //Catch any errors
		return -1; //We had one so return -1
	}

	return 0; //We had no errors, so return 0, success
}

int soar_sc_soar_command_thread(void* data) //Global function to execute soar commands on an agent
{
	//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	SetThreadName("Soar Command", GetCurrentThreadId());

	Soar_Command* cmd = reinterpret_cast<Soar_Command*>(data); //Convert the void* to a Soar_Command pointer.

	cmd->soar_link->ExecuteCommandLine(cmd->command.c_str()); //Execute the command in the Soar_Command on the agent given in the Soar_Command.

	delete cmd; //We're done so free up the cmd memory.

	return 0; //Return success (0).
}
