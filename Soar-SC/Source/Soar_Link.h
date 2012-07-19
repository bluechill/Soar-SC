#pragma once
#include <BWAPI.h>

#include "sml_Client.h"

#include <iostream>
#include <fstream>

#include <vector>

#include <windows.h>

#include "SDL/SDL_thread.h"
#include "SDL/SDL_mutex.h"

#include "Terrain-Analyzer.h"

// Remember not to use "Broodwar" in any global class constructor!

class Soar_Link : public BWAPI::AIModule
{
private:

	sml::Kernel* kernel;
	sml::Agent* agent;

	std::ofstream cout_redirect;
	std::streambuf* cout_orig_buffer;

	std::ofstream cerr_redirect;
	std::streambuf* cerr_orig_buffer;

	std::ofstream test_input_file;
	
	std::set<std::vector<std::string> > unwalkable_polygons;

	BWAPI::Unitset my_units;
	BWAPI::Unitset enemy_units;

	BWAPI::Unitset minerals;
	BWAPI::Unitset vesp_gas;

	SDL_Thread* soar_thread;
	bool done_updating;

	SDL_mutex* mu;

	std::set<std::string> svs_command_queue;
	std::set<sml::WMElement*> to_destroy_queue; 

	TerrainAnalyzer* analyzer;

	SDL_Thread* console_thread;
	std::vector<std::string> console_buffer;

	void RedirectIO();

	bool console;

	BWAPI::Unit* getUnitFromID(std::string id);
	BWAPI::Unit* getUnitFromID(int id);

	bool contains_id(int id, BWAPI::Unitset units);

public:
	// Virtual functions for callbacks, leave these as they are.
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
	// Everything below this line is safe to modify.

	Soar_Link();
	~Soar_Link();

	std::set<std::vector<BWAPI::Position> > find_connected_tiles(const std::vector<std::vector<bool> > &walkable_tiles);
	void update_map();

	//Sends all the resources to Soar.
	void add_resource(int id, int count, BWAPI::Position, BWAPI::UnitType type);
	void delete_resource(int id);

	void delete_unit(int id);

	void update_resources();
	void update_units();

	//To get around an issue related to slow starcraft
	bool should_die;

	bool done_updating_agent;
	int soar_agent_thread();

	const static std::string unit_box_verts;

	static float flip_one_d_point(const float &point, const bool &x_axis);

	void console_function();
	void print_soar(sml::smlPrintEventId id, void *d, sml::Agent *a, char const *m);

	void output_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase);
};

//Global stuff
extern void output_global_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase);
extern void printcb(sml::smlPrintEventId id, void *d, sml::Agent *a, char const *m);

//Thread globals
extern int thread_runner_soar(void* link);
extern int thread_runner_console(void *link);

//Misc globals
extern std::string strip(std::string s, std::string lc, std::string rc);
