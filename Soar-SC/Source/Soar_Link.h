#pragma once
#include <BWAPI.h>

#include "sml_Client.h"

#include <iostream>
#include <fstream>

// Remember not to use "Broodwar" in any global class constructor!

class Soar_Link : public BWAPI::AIModule
{
private:

	sml::Kernel* kernel;
	sml::Agent* agent;

	std::ofstream cout_redirect;
	std::streambuf* cout_orig_buffer;

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
};
