#ifndef Soar_SC_h
#define Soar_SC_h 1

//SDL Headers
#include "SDL/SDL_thread.h"
#include "SDL/SDL_mutex.h"

//STD C++ Headers
#include <queue>
#include <map>
#include <fstream>

//BWAPI Headers
#include <BWAPI.h>

//Event Headers
#include "Soar_Event.h"
#include "BWAPI_Event.h"

//Windows Headers
#include <Windows.h>

//Class Stubs
class Soar_Link;
class BWAPI_Link;

//Global Functions
extern void SetThreadName(const char *threadName, DWORD threadId);

class Soar_SC //Class which coordinates the events and the two links (Soar and BWAPI)
{
public:
	Soar_SC();

	void onUnload();
	~Soar_SC();

	Soar_Link* get_soar_link() { return soar_link; }
	BWAPI_Link* get_bwapi_link() { return bwapi_link; }

	void add_event(Soar_Event event); //Add an event, handles mutexes to prevent simultanous access
	void add_event(BWAPI_Event event); //Add a BWAPI event

	void signal_soar_updates(); //Warning will not return until done with queue
	void signal_bwapi_updates(); //Warning will not return until done with queue
	void set_should_run_forever(bool forever);

	void bwapi_thread_update(bool direct_call = true);
	void soar_thread_update(bool direct_call = true);

private:
	Soar_Link* soar_link;
	BWAPI_Link* bwapi_link;

	SDL_cond* soar_thread_condition;
	SDL_cond* bwapi_thread_condition;

	SDL_mutex* soar_event_mutex;
	SDL_mutex* bwapi_event_mutex;

	SDL_Thread* soar_event_thread;
	SDL_Thread* bwapi_event_thread;

	bool kill_threads; //When set to true, the threads will exit as soon as possible
	bool run_forever;

	typedef struct {
		std::queue<BWAPI_Event> queue;
		int count;
	} BWAPI_Event_Struct;

	std::queue<Soar_Event> soar_event_queue; 
	std::map<BWAPI::Unit*, BWAPI_Event_Struct* > bwapi_event_queue;

	//Misc Stuff
	std::ofstream cout_redirect; //Used for redirecting cout to a file
	std::streambuf* cout_orig_buffer; //The original buffer of cout

	std::ofstream cerr_redirect; //Used for redirecting cerr to a file
	std::streambuf* cerr_orig_buffer; //The original buffer of cerr
};

#endif
