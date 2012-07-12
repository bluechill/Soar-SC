#pragma once
#include <BWAPI.h>

#include "sml_Client.h"

#include <iostream>
#include <fstream>

#include <vector>

#include <windows.h>

#include "SDL/SDL_thread.h"
#include "SDL/SDL_mutex.h"

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
	void update_resources();
	void update_units();

	//To get around an issue related to slow starcraft
	bool should_die;

	bool done_updating_agent;
	int soar_agent_thread();

	const static std::string unit_box_verts;
};
