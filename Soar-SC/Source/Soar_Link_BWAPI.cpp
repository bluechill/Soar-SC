#include "Soar_Link.h" //Include the Soar Link class header

#include <stdio.h> //Include the standard c io header
#include <stdlib.h> //Also include the standard c library header
#include <time.h> //Include the time header for time structs

#include <vector> //For std::vector
#include <set> //For std::set
#include <sstream> //For std::stringstream

using namespace BWAPI; //Use the namespaces BWAPI, std, sml to allow the use of string instead of std::string or Agent* instead of sml::Agent* etc.
using namespace std;
using namespace sml;

void Soar_Link::onStart() //The "real" constructor in that this does the real work.  This is called when the game starts by BWAPI.
{
	if (kernel->HadError()) //Check if the kernel had an error
	{ //It did
		const char* msg = kernel->GetLastErrorDescription(); //Get the description of the error
		cout << "Soar: " << msg << endl; //Tell the user by the stdout and
		Broodwar->printf("Soar: %s", msg); //Through starcraft
		return; //Then return.
	}
	//No error

	agent = kernel->CreateAgent("Soar-SC"); //Create a new agent

	if (kernel->HadError()) //Make sure there wasn't an error creating the agent
	{
		const char* msg = kernel->GetLastErrorDescription(); //There was so get the error description
		cout << "Soar: " << msg << endl; //Then output to stdout
		Broodwar->printf("Soar: %s", msg); //And starcraft
		return; //Then return
	}
	//No error

	kernel->RegisterForAgentEvent(smlEVENT_AFTER_AGENT_REINITIALIZED, send_base_input_global, this);

	//Register for all the Soar events
	agent->RegisterForRunEvent(smlEVENT_AFTER_OUTPUT_PHASE, output_global_handler, this); //Output phase, called when the agent is after the output phase, ie. inbetween the input and output phases
	agent->RegisterForRunEvent(smlEVENT_AFTER_RUN_ENDS, misc_global_handler, this); //Called whenever the run ends for any reason, halt, interrupt, stop-soar, etc.
	agent->RegisterForRunEvent(smlEVENT_BEFORE_RUN_STARTS, misc_global_handler, this); //Called just before the run starts after a run or step etc. command

	agent->RegisterForPrintEvent(smlEVENT_PRINT, printcb, this); //Register for the print event to handle all output from the agent and other misc info like which thing is running etc.

	agent->ExecuteCommandLine("watch 1"); //Not strictly needed but for being verbose, watch 1 outputs all the decision stuff like what the agent is doing along with outputs via write etc.
	agent->ExecuteCommandLine("source Soar-SC/Soar-SC.soar"); //Load our agent's source into memory
	event_queue.set_agent(agent); //Set the agent in the event queue now that the agent is ready

	send_base_input(agent);

	//Create the initial soar runner thread
	soar_thread = SDL_CreateThread(thread_runner_soar, this);
	if (!soar_thread) //Check to make sure the thread exists
	{ //It doesn't so tell the user
		Broodwar->printf("Soar: Unable to create soar thread!");
		cout << "Soar: Unable to create soar thread!" << endl;
	}

	//Output the map to a file
	/*ofstream ifs("bwapi-data/logs/map-3.txt", ios::out);

	if (!ifs.is_open())
	return;

	for (int y = 0;y < map.size();y++)
	{
	for (int x = 0;x < map[y].size();x++)
	{
	string c = ".";

	if (!map[y][x])
	c = "A";

	ifs << c;
	}

	ifs << endl;
	}*/
}

void Soar_Link::onEnd(bool isWinner)
{
	// Called when the game ends
	if ( isWinner )
	{
		// Log your win here!
	}
}


void Soar_Link::onFrame()
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

	event_queue.bwapi_update();
}

void Soar_Link::onSendText(std::string text)
{
	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());
	cout << "Soar: " << text << endl;

	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!

}

void Soar_Link::onReceiveText(BWAPI::Player* player, std::string text)
{
	// Parse the received text
	cout << player->getName() << "said '" << text << "'" << endl;
	Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

void Soar_Link::onPlayerLeft(BWAPI::Player* player)
{

}

void Soar_Link::onNukeDetect(BWAPI::Position target)
{

}

void Soar_Link::onUnitDiscover(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitEvade(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitShow(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitHide(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitCreate(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitDestroy(BWAPI::Unit* unit)
{ //When a unit is destroyed, delete it from SVS and the input link
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
		delete_unit(unit);
		return;
	}
}

void Soar_Link::onUnitMorph(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitRenegade(BWAPI::Unit* unit)
{

}

void Soar_Link::onSaveGame(std::string gameName)
{
	cout << "The game was saved to \"" << gameName << "\"." << endl;
	Broodwar->printf("The game was saved to \"%s\".", gameName.c_str());
}

void Soar_Link::onUnitComplete(BWAPI::Unit *bw_unit)
{
	
}

void Soar_Link::add_resource(int bw_id, int count, BWAPI::Position position, BWAPI::UnitType type) //Add a resource
{
	Identifier* input_link = agent->GetInputLink(); //Get the input link

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
	ss << ((float)position.x)/32.0f << " " << flip_one_d_point(((float)position.y)/32.0f, false) << " 0";
	string position_svs = ss.str();
	ss.str("");

	ss << ((float)(type.dimensionLeft() + type.dimensionRight() + 1))/32.0f << " " << ((float)(type.dimensionUp() + type.dimensionDown() + 1))/32.0f << " 1";
	string size = ss.str();
	ss.str("");

	//Create the svs add command
	string svs_command = "a " + svs_object_id + " world v " + unit_box_verts + " p " + position_svs + " s " + size + " r 0 0 0";
	//Broodwar->printf("%s", svs_command.c_str());
	cout << svs_command << endl;

	//Send the svs command to the agent
	SDL_mutexP(mu);
	agent->SendSVSInput(svs_command);
	SDL_mutexV(mu);

	//Create the svs object id on the input link
	resource->CreateStringWME("svsobject", svs_object_id.c_str());
	
	//Output it to the log files
	test_input_file << "I-resources-" << name << ": ^svsobject " << svs_object_id << endl;

	test_input_file << "SVS-Actual: " << svs_command << endl;
}

void Soar_Link::delete_resource(int bw_id)
{
	Identifier* input_link = agent->GetInputLink(); //Get the input link

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

			test_input_file << "SVS-Actual: " << svs_command << endl;

			event_queue.add_event(Soar_Event(svs_command, true)); //Add the svs command to the queue
			event_queue.add_event(Soar_Event(id->GetChild(j))); //And add the wme to destroy to the queue

			break; //Break because we're done
		}
	}

	//Delete the resource from list
	SDL_mutexP(mu);
	Unitset::iterator it;
	if ((it = minerals.find(getUnitFromID(bw_id))) != minerals.end())
		minerals.erase(it);
	else if ((it = vesp_gas.find(getUnitFromID(bw_id))) != vesp_gas.end())
		vesp_gas.erase(it);
	SDL_mutexV(mu);
}

void Soar_Link::update_resources() //Update the resources
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

	Identifier* input_link = agent->GetInputLink();
	
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

	minerals_id->Update(minerals);
	gas_id->Update(gas);

	max_supplies_id->Update(total_supplies);
	used_supplies_id->Update(used_supplies);
}

void Soar_Link::add_unit(BWAPI::Unit* bw_unit) //Add a new unit
{
	Soar_Unit* soar_unit = new Soar_Unit(agent, bw_unit, this);

	my_units[bw_unit] = soar_unit;
}

void Soar_Link::delete_unit(BWAPI::Unit* unit) //Delete an existing unit
{
	map<Unit*, Soar_Unit*>::iterator it = my_units.find(unit);

	if (it == my_units.end())
		return;
	
	it->second->delete_unit(&event_queue, agent);

	delete it->second;

	my_units.erase(it);
}

void Soar_Link::update_units() //Update all player units
{
	Unitset units = Broodwar->self()->getUnits();

	for (Unitset::iterator it = units.begin();it != units.end();it++)
	{
		Unit* unit = (*it);

		if (!unit->isCompleted())
			continue;

		Soar_Unit* soar_unit = my_units[unit];

		if (soar_unit == NULL)
			add_unit(unit);
		else
			soar_unit->update(agent);
	}
}

Unit* Soar_Link::getUnitFromID(string id_string) //Retrieve a unit from an id, converts the string to an int then calls the int version
{
	int id;
	stringstream ss(id_string);
	ss >> id;

	return getUnitFromID(id);
}

Unit* Soar_Link::getUnitFromID(int id) //Calls the broodwar get unit method.
{
	return Broodwar->getUnit(id);
}

void Soar_Link::update_fogOfWar()
{
	size_t mapSize_x = Broodwar->mapWidth();
	size_t mapSize_y = Broodwar->mapHeight();

	static vector<pair<int, int> > visible_tiles;

	for (size_t x = 0;x < mapSize_x;x++)
	{
		for (size_t y = 0;y < mapSize_y;y++)
		{
			int id_x = x - x % 4;
			int id_y = size_t(flip_one_d_point(float(y - y % 4), false));

			pair<int, int> block = make_pair(id_x, id_y);

			bool found = false;
			for (vector<pair<int, int> >::iterator it = visible_tiles.begin();it != visible_tiles.end();it++)
			{
				if (it->first == id_x && it->second == id_y)
				{
					found = true;
					break;
				}
			}

			if (found)
				continue;

			if (Broodwar->isVisible(x, y))
			{
				Identifier* input_link = agent->GetInputLink();
				WMElement* elem = input_link->FindByAttribute("fog-tiles", 0);

				assert(elem == NULL);

				Identifier* fog_tiles = elem->ConvertToIdentifier();

				std::stringstream ss_x;
				ss_x << id_x;
				std::stringstream ss_y;
				ss_y << id_y;
				
				std::string svsobject_id = "fog" + ss_x.str() + ":" + ss_y.str();

				int children = fog_tiles->GetNumberChildren();
				for (int i = 0;i < children;i++)
				{
					Identifier* child = fog_tiles->GetChild(i)->ConvertToIdentifier();

					std::string svs_id = child->GetParameterValue("svsobject");
					if (svs_id != svsobject_id)
						continue;

					event_queue.add_event(Soar_Event(fog_tiles->GetChild(i)));

					std::string command = "d " + svsobject_id;
					
					event_queue.add_event(Soar_Event(command, true));

					visible_tiles.push_back(block);

					break;
				}
			}
		}
	}
}
