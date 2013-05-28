#ifndef SOAR_LINK_H
#define SOAR_LINK_H 1

//SDL Headers
#include "SDL/SDL_thread.h"
#include "SDL/SDL_mutex.h"

//SML Headers
#include "sml_Client.h"

//C++ Standard Library Headers
#include <iostream>
#include <fstream>
#include <vector>

//Windows Headers
#include <windows.h> //For windows related fuctions

//C Standard Library Headers
#include <time.h>

//Soar SC Headers
#include "Terrain.h"

#include <BWAPI.h>

class Soar_SC;
class Soar_Unit;

class Soar_Link //The AI class.  Inherits from AIModule to use BWAPI functions.
{
public:
	//Class stuff
	Soar_Link(Soar_SC* soar_sc_link);
	~Soar_Link();

	//Soar agent thread
	int soar_agent_thread();

	//Soar agent handlers
	void print_soar(sml::smlPrintEventId id, void *d, sml::Agent *a, char const *m);
	void output_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase);
	void misc_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase);

	void update_fogOfWar(float x_start, float y_start, float size_x, float size_y);

	//BWAPI Link Stuff
	//Getters & Setters
	void SendSVSInput(std::string input);
	void ExecuteCommandLine(std::string input);
	sml::Identifier* GetOutputLink();
	sml::Identifier* GetInputLink();
	sml::Kernel* GetKernel() { return kernel; }
	sml::Agent* GetAgent() { return agent; }

	//Soar Handlers
	void add_resource(int bw_id, int count, BWAPI::Position position, BWAPI::UnitType type);
	void delete_resource(int bw_id);
	void update_resource_count(int minerals, int gas, int total_supplies, int used_supplies);

	void update_update_unit_count(std::map<BWAPI::UnitType, unsigned int> unit_counts);

	void start_soar_run();
	void send_base_input(sml::Agent* agent, bool wait_for_analyzer);

	int get_decisions();
	void set_decisions(int new_count);

	Soar_Unit* soar_unit_from_svsobject_id(std::string svsobject_id);

private:
	std::vector<Soar_Unit*> fog_tiles;
	std::vector<Soar_Unit*> terrain_corners;

	int decisions; //Should use #pragma omp atomic for this

	Soar_SC* soar_sc_link;

	sml::Kernel* kernel; //Pointer to the soar kernel created
	sml::Agent* agent; //Pointer to the soar agent created

	SDL_Thread* soar_thread; //Variable for the thread for initially running the agent
	SDL_mutex* mu; //Variable for holding the mutex used by this classes's threads and event calls

	bool kill_threads;
	bool had_interrupt; //Used for detecting when the agent is stopped, set to true when it is by the misc handler, then it tells the event queue to update forever

	Terrain* terrain; //Terrain analyzer thread, analyzes the map and puts everything into SVS

	//Soar Helper Functions
	static sml::WMElement* get_child(std::string name, sml::Identifier* parent);
	static sml::WMElement* find_child_with_attribute_value(std::string attribute_name, std::string value, sml::Identifier* parent);
	static sml::WMElement* find_child_with_attribute_value(std::string child_name, std::string attribute_name, std::string value, sml::Identifier* parent);
};

//Global stuff, all just calls the respective Soar_Link function
extern void output_global_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase); //Handle for the output phase of Soar
extern void printcb(sml::smlPrintEventId id, void *d, sml::Agent *a, char const *m); //Print handler for Soar
extern void misc_global_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase); //Misc handler for Soar

//Thread globals, calls the Soar_Link function
extern int thread_runner_soar(void* link); //Initial thread for running the agent

extern void send_base_input_global(sml::smlAgentEventId id, void* pUserData, sml::Agent* pAgent); //Called at agent creation and agent reinit

extern void SetThreadName(const char *threadName, DWORD threadId);

#endif
