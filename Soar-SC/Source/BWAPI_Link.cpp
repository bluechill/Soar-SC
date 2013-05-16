//Soar SC Headers
#include "Soar_SC.h"
#include "Soar_Link.h"
#include "BWAPI_Link.h"
#include "Terrain.h"

using namespace std;
using namespace BWAPI;

BWAPI_Link::BWAPI_Link(Soar_SC* soar_sc_link)
{
	this->soar_sc_link = soar_sc_link;
}

BWAPI_Link::~BWAPI_Link()
{}

// functions for callbacks from BWAPI.
void BWAPI_Link::onStart()
{
	soar_sc_link->get_soar_link()->start_soar_run();
}

void BWAPI_Link::onEnd(bool isWinner)
{}

float BWAPI_Link::get_average_decisions()
{
	float total_frames = 0;
	int frame_count = last_50_decisions.size();

	if (frame_count < 24)
		return 0.0f;

	for (int i = 0;i < frame_count;++i)
		total_frames += last_50_decisions[i];

	total_frames /= frame_count;

	return total_frames;
}

void BWAPI_Link::onFrame()
{
	// Called once every game frame

	int decisions_last_frame = soar_sc_link->get_soar_link()->get_decisions();

	last_50_decisions.push_back(decisions_last_frame);

	if (last_50_decisions.size() > 50)
		last_50_decisions.erase(last_50_decisions.begin());

	float average_fps = Broodwar->getAverageFPS();
	int fps = Broodwar->getFPS();

	// Display the game frame rate as text in the upper left area of the screen
	Broodwar->drawTextScreen(10, 0,  "\x7 FPS: %d", fps );
	Broodwar->drawTextScreen(10, 15, "\x7 Average FPS: %f", average_fps);
	Broodwar->drawTextScreen(10, 30, "\x7 Decisions Per Second: %d", decisions_last_frame * fps);
	Broodwar->drawTextScreen(10, 45, "\x7 Average Decisions Per Second: %f", get_average_decisions() * average_fps);

	soar_sc_link->get_soar_link()->set_decisions(0);

	// Return if the game is a replay or is paused
	if ( Broodwar->isReplay() || Broodwar->isPaused() )
		return;

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
		return;

	soar_sc_link->bwapi_thread_update();
	//soar_sc_link->signal_bwapi_updates();
}

void BWAPI_Link::onSendText(std::string text)
{
	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());
	cout << "Soar: " << text << endl;
}

void BWAPI_Link::onReceiveText(BWAPI::Player* player, std::string text)
{
	// Parse the received text
	cout << player->getName() << "said '" << text << "'" << endl;
	Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

void BWAPI_Link::onPlayerLeft(BWAPI::Player* player)
{}

void BWAPI_Link::onNukeDetect(BWAPI::Position target)
{}

void BWAPI_Link::onUnitDiscover(BWAPI::Unit* unit)
{}

void BWAPI_Link::onUnitEvade(BWAPI::Unit* unit)
{}

void BWAPI_Link::onUnitShow(BWAPI::Unit* unit)
{}

void BWAPI_Link::onUnitHide(BWAPI::Unit* unit)
{
	std::map<Unit*, Soar_Unit*>::iterator enemy_it = enemy_units.find(unit);

	if (enemy_it == enemy_units.end())
		return;

	BWAPI::Position oldPos = BWAPI::Position(int(enemy_it->second->lastPosition().x * 32.0f), int(Terrain::flip_one_d_point(enemy_it->second->lastPosition().y, false) * 32.0f));
	hiddenUnitsPositions[enemy_it->second->get_id()] = oldPos;
}

void BWAPI_Link::onUnitCreate(BWAPI::Unit* unit)
{}

void BWAPI_Link::onUnitDestroy(BWAPI::Unit* unit)
{
	Unitset::iterator it;
	if ((it = minerals.find(unit)) != minerals.end()) //Check if it's a mineral
	{ //It is so delete it
		delete_resource(unit->getID());
		return;
	}

	if ((it = vesp_gas.find(unit)) != vesp_gas.end()) //Check if it's vesp gas
	{ //Then delete it
		delete_resource(unit->getID());
		return;
	}

	if (my_units.find(unit) != my_units.end()) //Check if it's a unit
	{ //It is so delete it using delete_unit
		delete_unit(unit, false);
		return;
	}

	if (enemy_units.find(unit) != enemy_units.end())
	{
		delete_unit(unit, true);
		return;
	}
}

void BWAPI_Link::onUnitMorph(BWAPI::Unit* unit)
{}

void BWAPI_Link::onUnitRenegade(BWAPI::Unit* unit)
{}

void BWAPI_Link::onSaveGame(std::string gameName)
{
	cout << "The game was saved to \"" << gameName << "\"." << endl;
	Broodwar->printf("The game was saved to \"%s\".", gameName.c_str());
}

void BWAPI_Link::onUnitComplete(BWAPI::Unit *unit)
{}


Unit* BWAPI_Link::getUnitFromID(string id_string) //Retrieve a unit from an id, converts the string to an int then calls the int version
{
	int id;
	stringstream ss(id_string);
	ss >> id;

	return getUnitFromID(id);
}

Unit* BWAPI_Link::getUnitFromID(int id) //Calls the broodwar get unit method.
{
	return Broodwar->getUnit(id);
}

void BWAPI_Link::delete_resource(int bw_id)
{
	//Delete the resource from list
	Unitset::iterator it;
	if ((it = minerals.find(getUnitFromID(bw_id))) != minerals.end())
		minerals.erase(it);
	else if ((it = vesp_gas.find(getUnitFromID(bw_id))) != vesp_gas.end())
		vesp_gas.erase(it);
}

void BWAPI_Link::add_unit(BWAPI::Unit* bw_unit, bool enemy) //Add a new unit
{
	Soar_Unit* soar_unit = new Soar_Unit(soar_sc_link, bw_unit, enemy);

	if (!enemy)
		my_units[bw_unit] = soar_unit;
	else
		enemy_units[bw_unit] = soar_unit;
}

void BWAPI_Link::delete_unit(BWAPI::Unit* unit, bool enemy) //Delete an existing unit
{
	map<Unit*, Soar_Unit*>::iterator it = my_units.find(unit);

	if (enemy)
	{
		map<Unit*, Soar_Unit*>::iterator enemy_it = enemy_units.find(unit);

		if (enemy_it == enemy_units.end())
			return;

		map<int, BWAPI::Position>::iterator hiddenUnits_it = hiddenUnitsPositions.find(enemy_it->second->get_id());

		if (hiddenUnits_it != hiddenUnitsPositions.end())
			hiddenUnitsPositions.erase(hiddenUnits_it);

		enemy_it->second->delete_unit();
		delete enemy_it->second;
		
		enemy_units.erase(enemy_it);
		
		return;
	}
	
	it->second->delete_unit();

	delete it->second;

	my_units.erase(it);
}

void BWAPI_Link::update_resources() //Update the resources
{
	Unitset visible_minerals = Broodwar->getMinerals();
	Unitset visible_vesp_gas = Broodwar->getGeysers();

	for (Unitset::iterator it = visible_minerals.begin();it != visible_minerals.end();it++)
	{
		if (minerals.find(*it) == minerals.end() && (*it)->getResources() != 0)
		{
			//Doesn't exist in my current list of visible minerals

			Unit* bw_unit = (*it);

			int size_y = bw_unit->getType().dimensionUp() + bw_unit->getType().dimensionDown() + 1;

			soar_sc_link->get_soar_link()->add_resource(bw_unit->getID(), bw_unit->getResources(), Position(bw_unit->getLeft(), bw_unit->getTop() + size_y), bw_unit->getType());
			minerals.insert(bw_unit);
		}
	}

	for (Unitset::iterator it = visible_vesp_gas.begin();it != visible_vesp_gas.end();it++)
	{
		if (vesp_gas.find(*it) == vesp_gas.end() && (*it)->getResources() != 0)
		{
			//Doesn't exist in my current list of visible vespian gas

			Unit* bw_unit = (*it);

			int size_y = bw_unit->getType().dimensionUp() + bw_unit->getType().dimensionDown() + 1;

			soar_sc_link->get_soar_link()->add_resource(bw_unit->getID(), bw_unit->getResources(), Position(bw_unit->getLeft(), bw_unit->getTop() + size_y), bw_unit->getType());
			vesp_gas.insert(bw_unit);
		}
	}

	int minerals = Broodwar->self()->minerals();
	int gas = Broodwar->self()->gas();

	int total_supplies = Broodwar->self()->supplyTotal();
	int used_supplies = Broodwar->self()->supplyUsed();

	soar_sc_link->get_soar_link()->update_resource_count(minerals, gas, total_supplies, used_supplies);
}

void BWAPI_Link::update_units() //Update all player units
{
	Playerset players = Broodwar->getPlayers();

	for (Playerset::iterator it = players.begin();it != players.end();it++)
	{
		Player* player = *it;
		Unitset units = player->getUnits();

		for (Unitset::iterator it = units.begin();it != units.end();it++)
		{
			Unit* unit = (*it);

			if (unit->getPlayer()->isEnemy(Broodwar->self()) || unit->getPlayer()->getID() == Broodwar->self()->getID())
			{
				bool isEnemy = unit->getPlayer()->isEnemy(Broodwar->self());

				if (!unit->isCompleted() && !isEnemy)
					continue;

				Soar_Unit* soar_unit;

				if (!isEnemy)
					soar_unit = my_units[unit];
				else
					soar_unit = enemy_units[unit];

				if (soar_unit == nullptr)
					add_unit(unit, isEnemy);
				else
					soar_unit->update();
			}
		}
	}
}

std::map<BWAPI::Unit*, Soar_Unit*> BWAPI_Link::get_units()
{
	std::map<BWAPI::Unit*, Soar_Unit*> result;
	result.insert(my_units.begin(), my_units.end());
	result.insert(enemy_units.begin(), enemy_units.end());

	return result;
}
