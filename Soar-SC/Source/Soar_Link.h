#ifndef SOAR_LINK_H //Make sure this header is never included more than once to prevent multiple definitions
#define SOAR_LINK_H 1

#include <BWAPI.h> //For connection to Starcraft

#include "sml_Client.h" //For SML Agent functions (Soar functions)

#include <iostream> //For io functions and classes like cout and cerr
#include <fstream> //For file functions like fstream

#include <vector> //For std::vector

#include <windows.h> //For windows related fuctions

#include "SDL/SDL_thread.h" //For SDL threading
#include "SDL/SDL_mutex.h" //For SDL mutexes

#include "Terrain-Analyzer.h" //For the Terrain Analyzer
#include "Soar_Console.h" //For the Soar Console

#include "Events.h" //For the event queuee

#include "Soar_Unit.h"

// Remember not to use "Broodwar" in any global class constructor!

class Soar_Link : public BWAPI::AIModule //The AI class.  Inherits from AIModule to use BWAPI functions.
{
public:
	// Virtual functions for callbacks from BWAPI.
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player* player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player* player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit* unit);
	virtual void onUnitEvade(BWAPI::Unit* unit);
	virtual void onUnitShow(BWAPI::Unit* unit);
	virtual void onUnitHide(BWAPI::Unit* unit);
	virtual void onUnitCreate(BWAPI::Unit* unit);
	virtual void onUnitDestroy(BWAPI::Unit* unit);
	virtual void onUnitMorph(BWAPI::Unit* unit);
	virtual void onUnitRenegade(BWAPI::Unit* unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit *unit);

	Soar_Link(); //Constructor for class.  Takes nothing. 
	~Soar_Link(); //Deconstructor

	//Sends all the resources to Soar.
	void add_resource(int id, int count, BWAPI::Position, BWAPI::UnitType type); //Add a resource in SVS and on the input link
	void delete_resource(int id); //Delete a resource from SVS and the input link

	void add_unit(BWAPI::Unit* unit); //Add a unit to the input link and SVS
	void delete_unit(BWAPI::Unit* unit); //Delete a unit from the input link and SVS

	void update_resources(); //Update the resources of the player (AI) (Agent)
	void update_units(); //Update the units of the player (AI) (Agent) (and eventually visible enemy positions etc.)

	bool should_die; //Set to true to kill the threads spawned

	int soar_agent_thread(); //Initial thread for running the agent.  Spawned after the Terrain Analyzer is done

	const static std::string unit_box_verts; //A string for a generic unit box verts

	static float flip_one_d_point(const float &point, const bool &x_axis); //Flip a point from Starcraft Top Left being 0,0 to Bottom Left being 0,0 for sending stuff to the SVS Viewer

	void print_soar(sml::smlPrintEventId id, void *d, sml::Agent *a, char const *m); //Function to handle print events from Soar

	void output_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase); //Function for handling the output phase of the Agent
	void misc_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase); //Function for misc events like when a run ends or starts

private:
	sml::Kernel* kernel; //Pointer to the soar kernel created
	sml::Agent* agent; //Pointer to the soar agent created

	std::ofstream cout_redirect; //Used for redirecting cout to a file
	std::streambuf* cout_orig_buffer; //The original buffer of cout

	std::ofstream cerr_redirect; //Used for redirecting cerr to a file
	std::streambuf* cerr_orig_buffer; //The original buffer of cerr

	std::ofstream test_input_file; //File for test input to SVS

	std::map<BWAPI::Unit*, Soar_Unit*> my_units; //A set containing all the units of the player (AI) (Agent)
	std::map<BWAPI::Unit*, Soar_Unit*> enemy_units; //A set for containing all the enemy units, currently not used

	BWAPI::Unitset minerals; //A set for containing all the minerals seen
	BWAPI::Unitset vesp_gas; //A set for containing all the vesp gas geysers seen

	SDL_Thread* soar_thread; //Variable for the thread for initially running the agent

	SDL_mutex* mu; //Variable for holding the mutex used by this classes's threads and event calls

	Events event_queue; //The event queue variable
	bool had_interrupt; //Used for detecting when the agent is stopped, set to true when it is by the misc handler, then it tells the event queue to update forever
	
	TerrainAnalyzer* analyzer; //Terrain analyzer thread, analyzes the map and puts everything into SVS

	BWAPI::Unit* getUnitFromID(std::string id); //Get a unit from a string containing the id, converts the string to an int then calls the int version
	BWAPI::Unit* getUnitFromID(int id); //Returns the unit (in the list of units it has) associated with an ID, if it can't finds it returns NULL

	bool contains_id(int id, BWAPI::Unitset units); //Check whether a given set of units has a unit with the ID given

	Soar_Console* console; //Pointer to the Soar console class
};

//Global stuff, all just calls the respective Soar_Link function
extern void output_global_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase); //Handle for the output phase of Soar
extern void printcb(sml::smlPrintEventId id, void *d, sml::Agent *a, char const *m); //Print handler for Soar
extern void misc_global_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase); //Misc handler for Soar

//Thread globals, calls the Soar_Link function
extern int thread_runner_soar(void* link); //Initial thread for running the agent

#endif
