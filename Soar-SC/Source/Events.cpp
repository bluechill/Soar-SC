#define EVENTS_PRIV 1 //Define that we're using private events

#include "Events.h" //Include the Events header

#include "Soar_Console.h" //Include the Soar_Console header
#include "Soar_Link.h" //Include the Soar Link header

#include <string> //For std::string

#include "SDL/SDL.h"

using namespace std; //Say that we're using the standard namespace so we don't have to do std::string and can just type string

Events::Events(Soar_Console* console, Soar_Link* link) //The constructor of the event queue
{
	mu = SDL_CreateMutex(); //Create the mutex to be used for preventing simultanous access between event thread and everything else

	if (mu == NULL) //Make sure it's valid
	{
		std::cerr << "Unable to create mutex for event thread, things will probably go bad.... exiting...." << std::endl; //Otherwise tell the user
		exit(7331); //And exit
	}

	condition = SDL_CreateCond(); //Create a condition variable for suspending the event thread until it's needed

	if (condition == NULL) //Make sure the condition variable is valid
	{
		std::cerr << "Unable to create condition variable!" << std::endl; //Otherwise tell the user
		exit(7332); //And exit
	}

	should_die = false; //Set to false so the event thread doesn't immediately exit

	this->console = console; //Set the internal variable pointer to the console
	this->link = link; //Set the internal variable pointer to the link

	interrupted = false; //Make sure this is initially false otherwise the event thread will immediately start executing forever

	agent = NULL; //Set our agent to NULL

	events_thread = SDL_CreateThread(events_global_thread, "Events Thread", this); //Spawn the event queue thread
}

Events::~Events() //Dealloc the class
{
	should_die = true; //Tell the thread to die

	SDL_CondSignal(condition); //Signal the thread to start executing (and die)
	SDL_WaitThread(events_thread, NULL); //Wait until the thread has executed

	if (mu != NULL) //Then make sure the mutex isn't NULL
	{
		SDL_DestroyMutex(mu); //And since it isn't, destroy the mutex
		mu = NULL; //And set it to NULL
	}
}

void Events::update(bool lock) //Update function.  Set lock to true to lock a mutex otherwise it won't lock a mutex
{
	if (lock) //If we're to lock a mutex
		SDL_mutexP(mu); //Lock it

	while (!event_queue.empty()) //While our queue isn't empty
	{
		Soar_Event e = event_queue.front(); //Get the first event
		event_queue.pop_front(); //Then remove it from the queue

		switch (e.get_type()) //Switch based on the type of the event
		{
		case Soar_Event::SVS_Command: //If it's a SVS Command
			{
				agent->SendSVSInput(*e.get_command()); //Send it to the agent
				break; //Then break out of the switch statement
			}
		case Soar_Event::Console_Input: //If it's Console Input
			{
				if (console != NULL)
					console->recieve_input(*e.get_command()); //"echo" the command to the console

				if (e.get_command()->find("run") == 0 || e.get_command()->find("time") == 0) //Check if it's a run command or a time command
				{
					Soar_Command* cmd = new Soar_Command; //Since it is, create a new Soar_Command struct for the thread
					cmd->a = agent; //Set the agent for the thread
					cmd->command = *e.get_command(); //Set the command for the thread

					SDL_CreateThread(soar_command_thread, "Soar Run Thread", cmd); //Create a thread to execute the run command.  A run command never returns until the run stops.  This means that unless it's called from a seperate thread we will never be able to get info about the agent or stop the agent until it stops on it's own.
				}
				else //Otherwise
				{
					string output = agent->ExecuteCommandLine(e.get_command()->c_str()); //Execute it now
					
					if (console != NULL)
						console->recieve_input(output); //And then send the output to the console
				}

				break; //Then break
			}
		case Soar_Event::WME_Destroy: //If it's a destroy command
			{
				e.get_element()->DestroyWME(); //Destroy the WMElement
				break; //Then break
			}
		case Soar_Event::New_Unit: //If we're supposed to add a unit
			{
				BWAPI::Unit* unit = e.get_unit(); //Get the unit to add
				link->add_unit(unit); //Then add it
				break; //Then break
			}
		default: //If it's a default, handle it as an error.  This is never possible unless there is memory corruption in which case more things than just this will go wrong.
			{
				cerr << "Possible memory corruption! Event Type of '" << e.get_type() << "'" << endl; //Tell the user
				break; //Then break
			}
		}
	}
	
	if (lock) //If we locked the mutex
		SDL_mutexV(mu); //Unlock it because we're done
}

void Events::add_event(Soar_Event event) //Function to add events
{
	SDL_mutexP(mu); //Lock the mutex to prevent simultanous access
	event_queue.push_back(event); //Add the event to the internal queue
	SDL_mutexV(mu); //Then unlock the mutex
}

void Events::set_agent(sml::Agent* agent) //Set the agent to execute commands on in the event queue
{
	this->agent = agent; //Set our internal pointer to the given one
}

bool Events::update_forever() //Tell the event thread to update forever until update_only_on_calls() or should_die is set to true
{
	SDL_mutexP(mu); // Lock the event mutex in case the thread is still running
	interrupted = true; //Set interrupted to true to make sure the event queue will continue.  We have this variable because otherwise it could signal but then immediately need to not execute.
	SDL_mutexV(mu); //Unlock the event mutex
	SDL_CondSignal(condition); //Signal the thread

	return true; //Return that everything was successful
}

bool Events::update_only_on_calls() //Tell the event thread to stop executing and only execute when signaled
{
	SDL_mutexP(mu); //Lock the event mutex in case the thread is running
	interrupted = false; //Set interrupted to false, this will trigger a while loop to enter causing SDL_CondWait to suspend the thread
	SDL_mutexV(mu); //Unlock the mutex

	return true; //Return that everything was successful
}

void Events::update_thread() //Function called for the event thread
{
	while (true) //Infinite loop!
	{
		SDL_mutexP(mu); //Lock the mutex
		while (!interrupted) //Check if we're interrupted, if we're not
			SDL_CondWait(condition, mu); //Unlock the mutex and suspend the thread until we're signaled to continue.  Then, once signaled lock the mutex again

		if (should_die) //Make sure we aren't supposed to die
		{
			SDL_mutexV(mu); //We are so unlock the mutex
			return; //Then exit the thread
		}

		update(false); //Update the thread but make sure the function doesn't lock the mutex already locked
		
		SDL_mutexV(mu); //Unlock the mutex since we're done with it for now
	}
}

int events_global_thread(void* data) //Global function called by SDL.  The void* is a Soar_Link this pointer.
{
	try { //Try executing the event queue
		reinterpret_cast<Events*>(data)->update_thread(); //Convert the void* to a Soar_Link pointer, then call update_thread() to start doing the job the thread is supposed to, handle events
	}
	catch(...) { //Catch any errors
		return -1; //We had one so return -1
	}

	return 0; //We had no errors, so return 0, success
}

int soar_command_thread(void* data) //Global function to execute soar commands on an agent
{
	Soar_Command* cmd = reinterpret_cast<Soar_Command*>(data); //Convert the void* to a Soar_Command pointer.

	cmd->a->ExecuteCommandLine(cmd->command.c_str()); //Execute the command in the Soar_Command on the agent given in the Soar_Command.

	delete cmd; //We're done so free up the cmd memory.

	return 0; //Return success (0).
}

SDL_mutex* Events::get_mutex() //Function to return the Event's mutex.
{
	return mu; //Return it.
}