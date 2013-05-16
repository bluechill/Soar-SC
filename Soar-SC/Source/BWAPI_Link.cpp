//SML Headers
#include "sml_Client.h"

//Soar SC Headers
#include "Soar_SC.h"
#include "Soar_Link.h"
#include "BWAPI_Link.h"
#include "Terrain.h"

using namespace std;
using namespace BWAPI;
using namespace sml;

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

void BWAPI_Link::onFrame()
{
	// Called once every game frame

	// Display the game frame rate as text in the upper left area of the screen
	Broodwar->drawTextScreen(200, 0,  "FPS: %d", Broodwar->getFPS() );
	Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS() );

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


void BWAPI_Link::add_resource(int bw_id, int count, BWAPI::Position position, BWAPI::UnitType type) //Add a resource
{
	Identifier* input_link = soar_sc_link->get_soar_link()->GetInputLink(); //Get the input link

	Identifier* id; //Variable for the resources identifier
	if (!input_link->FindByAttribute("resources", 0)) //Check if it exits
	{ //If it doesn't then create it 
		Broodwar->printf("WARNING: No 'units' identifier on the input link! Creating....");
		cout << "WARNING: No 'units' identifier on the input link! Creating...." << endl;

		id = input_link->CreateIdWME("resources");
	}
	else //Otherwise
		id = input_link->FindByAttribute("resources", 0)->ConvertToIdentifier(); //Use the first existing one

	string name; //Variable for the type

	if (type.getName().find("Mineral") != string::npos) //If it's a mineral
		name = "mineral"; //Set the name to mineral
	else //Otherwise
		name = "vesp-gas"; //Set the name to vesp gas

	Identifier* resource = id->CreateIdWME(name.c_str()); //Create a new resource Identifier for the resrouce

	resource->CreateIntWME("id", bw_id); //Set the id
	resource->CreateIntWME("count", count); //Set the number of minerals it holds

	string svs_object_id = type.getName(); //Set the svs id to be the type's name
	svs_object_id.erase(remove_if(svs_object_id.begin(), svs_object_id.end(), isspace), svs_object_id.end()); //Remove all the spaces

	//Add the id of the resource to the id
	stringstream ss;
	ss << bw_id;
	svs_object_id += ss.str();
	ss.str("");
	//Flip the point so "north" isn't negative y
	ss << ((float)position.x)/32.0f << " " << Terrain::flip_one_d_point(((float)position.y)/32.0f, false) << " 0";
	string position_svs = ss.str();
	ss.str("");

	ss << ((float)(type.dimensionLeft() + type.dimensionRight() + 1))/32.0f << " " << ((float)(type.dimensionUp() + type.dimensionDown() + 1))/32.0f << " 1";
	string size = ss.str();
	ss.str("");

	//Create the svs add command
	string svs_command = "a " + svs_object_id + " world v " + Terrain::unit_box_verts + " p " + position_svs + " s " + size + " r 0 0 0";
	//Broodwar->printf("%s", svs_command.c_str());
	cout << svs_command << endl;

	//Send the svs command to the agent
	soar_sc_link->get_soar_link()->SendSVSInput(svs_command);

	//Create the svs object id on the input link
	resource->CreateStringWME("svsobject", svs_object_id.c_str());
}

void BWAPI_Link::delete_resource(int bw_id)
{
	Identifier* input_link = soar_sc_link->get_soar_link()->GetInputLink(); //Get the input link

	//Check for the existence of the resources identifier, if it doesn't exist, create it

	Identifier* id;

	if (!input_link->FindByAttribute("resources", 0))
	{
		cout << "ERROR: No 'resources' identifier on the input link! Creating...." << endl;

		id = input_link->CreateIdWME("resources");
	}
	else
		id = input_link->FindByAttribute("resources", 0)->ConvertToIdentifier();

	for (int j = 0;j < id->GetNumberChildren();j++) //Search for the existence of the given id in the resource list
	{
		Identifier* unit;
		if (!id->GetChild(j)->IsIdentifier())
			continue;
		else
			unit = id->GetChild(j)->ConvertToIdentifier();

		//Check the id against the given one
		if (unit->FindByAttribute("id", 0)->ConvertToIntElement()->GetValue() == bw_id)
		{
			//It is the same so delete the resource from svs and the input link
			string svs_object_id = unit->FindByAttribute("svsobject", 0)->ConvertToStringElement()->GetValue();

			string svs_command = "d " + svs_object_id;

			soar_sc_link->add_event(Soar_Event(svs_command, true)); //Add the svs command to the queue
			soar_sc_link->add_event(Soar_Event(id->GetChild(j))); //And add the wme to destroy to the queue

			break; //Break because we're done
		}
	}

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

			add_resource(bw_unit->getID(), bw_unit->getResources(), Position(bw_unit->getLeft(), bw_unit->getTop() + size_y), bw_unit->getType());
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

			add_resource(bw_unit->getID(), bw_unit->getResources(), Position(bw_unit->getLeft(), bw_unit->getTop() + size_y), bw_unit->getType());
			vesp_gas.insert(bw_unit);
		}
	}

	int minerals = Broodwar->self()->minerals();
	int gas = Broodwar->self()->gas();

	int total_supplies = Broodwar->self()->supplyTotal();
	int used_supplies = Broodwar->self()->supplyUsed();

	Identifier* input_link = soar_sc_link->get_soar_link()->GetInputLink();
	
	//Update the resource count
	IntElement* minerals_id;
	if (!input_link->FindByAttribute("minerals", 0))
		minerals_id = input_link->CreateIntWME("minerals", 0)->ConvertToIntElement();
	else
		minerals_id = input_link->FindByAttribute("minerals", 0)->ConvertToIntElement();

	IntElement* gas_id;
	if (!input_link->FindByAttribute("gas", 0))
		gas_id = input_link->CreateIntWME("gas", 0)->ConvertToIntElement();
	else
		gas_id = input_link->FindByAttribute("gas", 0)->ConvertToIntElement();

	IntElement* max_supplies_id;
	if (!input_link->FindByAttribute("total-supplies", 0))
		max_supplies_id = input_link->CreateIntWME("total-supplies", 0)->ConvertToIntElement();
	else
		max_supplies_id = input_link->FindByAttribute("total-supplies", 0)->ConvertToIntElement();

	IntElement* used_supplies_id;
	if (!input_link->FindByAttribute("used-supplies", 0))
		used_supplies_id = input_link->CreateIntWME("used-supplies", 0)->ConvertToIntElement();
	else
		used_supplies_id = input_link->FindByAttribute("used-supplies", 0)->ConvertToIntElement();

	if (minerals_id->GetValue() != minerals)
		minerals_id->Update(minerals);

	if (gas_id->GetValue() != gas)
		gas_id->Update(gas);

	if (max_supplies_id->GetValue() != total_supplies)
		max_supplies_id->Update(total_supplies);

	if (used_supplies_id->GetValue() != used_supplies)
		used_supplies_id->Update(used_supplies);
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

				if (soar_unit == NULL)
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
