#include "Soar_Link.h" //Include the Soar Link class header

using namespace BWAPI; //Use the namespaces BWAPI, sml, and std so we don't have to write out the full commands, ie. string instead of std::string or Agent* instead of sml::Agent*
using namespace sml;
using namespace std;

Soar_Link::Soar_Link() //Constructor for the Soar Link class
	: cout_redirect("bwapi-data/logs/stdout.txt"), //Initialize the replacement cout buffer with the path of the log file
	cerr_redirect("bwapi-data/logs/stderr.txt"), //Initialize the replacement cerr buffer with the path of the cerr log file
	test_input_file("bwapi-data/logs/test_input.txt"), //Initialize the test SVS input buffer with the path of the test svs log file
	event_queue(NULL, this) //Initialize the event queue with a temporary NULL pointer for the console and a this pointer for the Soar Link
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

	kernel = Kernel::CreateKernelInNewThread(); //Create a new Soar kernel (agent spawner) in a new thread
	//kernel = Kernel::CreateRemoteConnection(false, "35.0.136.73", 12121);

	mu = SDL_CreateMutex(); //Create a new mutex to prevent simultanous access to shared objects
	
	//console = new Soar_Console(&event_queue); //Create a new soar console with a pointer to the event queue
	console = NULL;

	event_queue.set_console(console); //Set the event queue pointer to the soar console pointer

	should_die = false; //Make sure the initial soar agent run thread doesn't immediately die when it is created
	had_interrupt = false; //Make sure the misc handler will properly handle run starts and stops by setting the default value
}

Soar_Link::~Soar_Link() //Deconstructor
{
	should_die = true; //Tell the soar agent run thread to die if it is still running

	event_queue.add_event(Soar_Event("stop", false)); //Execute a stop command to stop the agent

	SDL_WaitThread(soar_thread, NULL); //Wait for the soar agent run thread to die if it's till running
	SDL_DestroyMutex(mu); //Then destroy the mutex

	kernel->DestroyAgent(agent); //Shut down the agent
	kernel->Shutdown(); //Then shut down the soar kernel

	cout.rdbuf(cout_orig_buffer); //Redirect the cout buffer back to prevent errors after deallocation
	cerr.rdbuf(cerr_orig_buffer); //Do the same for the cerr buffer

	if (console != NULL)
		delete console; //Then dealloc the console
}
