#ifndef BWAPI_Link_h
#define BWAPI_Link_h 1
#include <BWAPI.h>

#include "Soar_Unit.h"

class Soar_SC;

class BWAPI_Link : public BWAPI::AIModule //The AI class.  Inherits from AIModule to use BWAPI functions.
{
public:
	BWAPI_Link(Soar_SC* soar_sc_link);
	~BWAPI_Link();

	// functions for callbacks from BWAPI.
	void onStart();
	void onEnd(bool isWinner);
	void onFrame();
	void onSendText(std::string text);
	void onReceiveText(BWAPI::Player* player, std::string text);
	void onPlayerLeft(BWAPI::Player* player);
	void onNukeDetect(BWAPI::Position target);
	void onUnitDiscover(BWAPI::Unit* unit);
	void onUnitEvade(BWAPI::Unit* unit);
	void onUnitShow(BWAPI::Unit* unit);
	void onUnitHide(BWAPI::Unit* unit);
	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);
	void onUnitMorph(BWAPI::Unit* unit);
	void onUnitRenegade(BWAPI::Unit* unit);
	void onSaveGame(std::string gameName);
	void onUnitComplete(BWAPI::Unit *unit);

	BWAPI::Unit* getUnitFromID(std::string id); //Get a unit from a string containing the id, converts the string to an int then calls the int version
	BWAPI::Unit* getUnitFromID(int id); //Returns the unit (in the list of units it has) associated with an ID, if it can't finds it returns nullptr

	bool contains_id(int id, BWAPI::Unitset units); //Check whether a given set of units has a unit with the ID given

	void delete_resource(int id);

	void add_unit(BWAPI::Unit* unit, bool enemy);
	void delete_unit(BWAPI::Unit* unit, bool enemy);

	void update_resources();
	void update_units();

	std::map<int, BWAPI::Position> get_hidden_map() { return hiddenUnitsPositions; }
	std::map<BWAPI::Unit*, Soar_Unit*> get_units();

private:
	Soar_SC* soar_sc_link;

	std::map<BWAPI::Unit*, Soar_Unit*> my_units; //A set containing all the units of the player (AI) (Agent)
	std::map<BWAPI::Unit*, Soar_Unit*> enemy_units; //A set for containing all the enemy units, currently not used
	std::map<int, BWAPI::Position> hiddenUnitsPositions;

	BWAPI::Unitset minerals; //A set for containing all the minerals seen
	BWAPI::Unitset vesp_gas; //A set for containing all the vesp gas geysers seen
};

#endif
