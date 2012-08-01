#include "Soar_Link.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <vector>
#include <set>
#include <sstream>

using namespace BWAPI;
using namespace std;
using namespace sml;

void Soar_Link::onStart()
{
	if (kernel->HadError())
	{
		const char* msg = kernel->GetLastErrorDescription();
		cout << "Soar: " << msg << endl;
		Broodwar->printf("Soar: %s", msg);
		return;
	}

	agent = kernel->CreateAgent("Soar-SC");

	if (kernel->HadError())
	{
		const char* msg = kernel->GetLastErrorDescription();
		cout << "Soar: " << msg << endl;
		Broodwar->printf("Soar: %s", msg);
		return;
	}

	agent->RegisterForRunEvent(smlEVENT_AFTER_OUTPUT_PHASE, output_global_handler, this);
	agent->RegisterForRunEvent(smlEVENT_AFTER_INTERRUPT, misc_global_handler, this);
	agent->RegisterForRunEvent(smlEVENT_BEFORE_RUNNING, misc_global_handler, this);

	agent->RegisterForPrintEvent(smlEVENT_PRINT, printcb, this);

	agent->ExecuteCommandLine("source Soar-SC/Soar-SC.soar");
	event_queue.set_agent(agent);

	stringstream ss;
	ss << Broodwar->mapWidth();
	string map_width_as_string = ss.str();
	ss.str("");
	ss << Broodwar->mapHeight();
	string map_height_as_string = ss.str();
	ss.str("");

	//Bottom barrier
	string svs_command_1 = "a -x0 world v " + unit_box_verts + " p 0 -1 0 s " + map_width_as_string + " 1 1";

	test_input_file << "SVS-Actual: " << svs_command_1 << endl;

	agent->SendSVSInput(svs_command_1);

	//Top barrier
	string svs_command_2 = "a x0 world v " + unit_box_verts + " p 0 " + map_height_as_string + " 0 s " + map_width_as_string + " 1 1";

	test_input_file << "SVS-Actual: " << svs_command_2 << endl;

	agent->SendSVSInput(svs_command_2);

	//Left barrier
	string svs_command_3 = "a -y0 world v " + unit_box_verts + " p -1 0 0 s 1 " + map_height_as_string + " 1";

	test_input_file << "SVS-Actual: " << svs_command_3 << endl;

	agent->SendSVSInput(svs_command_3);

	//Right barrier
	string svs_command_4 = "a y0 world v " + unit_box_verts + " p " + map_width_as_string + " 0 0 s 1 " + map_height_as_string + " 1";

	test_input_file << "SVS-Actual: " << svs_command_4 << endl;

	agent->SendSVSInput(svs_command_4);

	cout << "Soar-SC is running." << endl;
	Broodwar->printf("Soar-SC is running.");

	vector<vector<bool> > map;
	size_t map_size_x = Broodwar->mapWidth() * 4;
	size_t map_size_y = Broodwar->mapHeight() * 4;

	vector<vector<bool> > build_tiles_map;

	for (int y = 0;y < int(map_size_y);y++)
	{
		vector<bool> map_y;

		int last_16 = 0;
		for (int x = 0;x < int(map_size_x);x++)
		{
			map_y.push_back(Broodwar->isWalkable(x,y));
			if (x % 4 == 0)
			{
				if ((x-3) > 0 && !map_y[x-3])
					last_16++;

				if ((x-2) > 0 && !map_y[x-2])
					last_16++;

				if ((x-2) > 0 && !map_y[x-1])
					last_16++;

				if (y != 0 && !map_y[x])
					last_16++;
			}

			if (y % 4 == 0)
			{
				if ((y-3) > 0 && !map[y-3][x])
					last_16++;

				if ((y-2) > 0 && !map[y-2][x])
					last_16++;

				if ((y-1) > 0 && !map[y-1][x])
					last_16++;

				if (!map_y[x])
					last_16++;
			}

			if (y % 4 == 0 && x % 4 == 0 && last_16 > 0)
			{
				if (y == 0 && x == 0)
				{
					map_y[x] = false;

					last_16 = 0;
					continue;
				}
				else if (y == 0)
				{
					for (int j = 3;j >= 0;j--)
						map_y[x-j] = false;

					continue;
				}

				for (int i = 3;i >= 0;i--)
				{
					if (x == 0)
					{
						if (i == 0)
							map_y[x] = false;
						else
							map[y-i][x] = false;

						last_16 = 0;

						continue;
					}

					for (int j = 3;j >= 0;j--)
					{
						if (i == 0)
							map_y[x-j] = false;
						else
							map[y-i][x-j] = false;
					}
				}

				last_16 = 0;
			}
		}

		map.push_back(map_y);
	}

	//Clean up the map even more

	for (int y = 0;y < int(map_size_y);y += 4)
	{
		for (int x = 0;x < int(map_size_x);x += 4)
		{
			if (map[y][x])
			{
				int surrounding = 0;

				int distance = 5;

				if (y + distance < int(map.size()) && !map[y+distance][x])
					surrounding++;
				else if (y + distance - 1 < int(map.size()) && !map[y+distance-1][x])
					surrounding++;

				if (y - distance >= 0 && !map[y-distance][x])
					surrounding++;
				else if (y - distance + 1 >= 0 && !map[y-distance+1][x])
					surrounding++;

				if (x + distance < int(map[y].size()) && !map[y][x+distance])
					surrounding++;
				else if (x + distance - 1 < int(map[y].size()) && !map[y][x+distance-1])
					surrounding++;

				if (x - distance >= 0 && !map[y][x-distance])
					surrounding++;
				else if (x - distance + 1 >= 0 && !map[y][x-distance+1])
					surrounding++;

				//Diagonals

				if (y + distance < int(map.size()) && x + distance < int(map[y+distance].size()) && !map[y+distance][x+distance])
					surrounding++;
				else if (y + distance - 1 < int(map.size()) && x + distance < int(map[y+distance-1].size()) && !map[y+distance-1][x+distance])
					surrounding++;
				else if (y + distance - 1 < int(map.size()) && x + distance - 1 < int(map[y+distance-1].size()) && !map[y+distance-1][x+distance-1])
					surrounding++;
				else if (y + distance < int(map.size()) && x + distance - 1 < int(map[y+distance].size()) && !map[y+distance][x+distance-1])
					surrounding++;

				if (y + distance < int(map.size()) && x - distance >= 0 && !map[y+distance][x-distance])
					surrounding++;
				else if (y + distance - 1 < int(map.size()) && x - distance >= 0 && !map[y+distance-1][x-distance])
					surrounding++;
				else if (y + distance - 1 < int(map.size()) && x - distance + 1 >= 0 && !map[y+distance-1][x-distance+1])
					surrounding++;
				else if (y + distance < int(map.size()) && x - distance + 1 >= 0 && !map[y+distance][x-distance+1])
					surrounding++;

				if (y - distance >= 0 && x + distance < int(map[y-distance].size()) && !map[y-distance][x+distance])
					surrounding++;
				else if (y - distance + 1 >= 0 && x + distance < int(map[y-distance+1].size()) && !map[y-distance+1][x+distance])
					surrounding++;
				else if (y - distance + 1 >= 0 && x + distance - 1 < int(map[y-distance+1].size()) && !map[y-distance+1][x+distance-1])
					surrounding++;
				else if (y - distance >= 0 && x + distance - 1 < int(map[y-distance].size()) && !map[y-distance][x+distance-1])
					surrounding++;

				if (y - distance >= 0 && x - distance >= 0 && !map[y-distance][x-distance])
					surrounding++;
				else if (y - distance + 1 >= 0 && x - distance >= 0 && !map[y-distance+1][x-distance])
					surrounding++;
				else if (y - distance + 1 >= 0 && x - distance + 1 >= 0 && !map[y-distance+1][x-distance+1])
					surrounding++;
				else if (y - distance >= 0 && x - distance + 1 >= 0 && !map[y-distance][x-distance+1])
					surrounding++;

				if (surrounding > 4) //Greater than 50%
				{
					for (int i = 3;i >= 0;i--)
					{
						for (int j = 3;j >= 0;j--)
							map[y-i][x-j] = false;
					}
					continue;
				}
			}
		}
	}

	analyzer = new TerrainAnalyzer(map, agent, mu);
	analyzer->analyze();

	soar_thread = SDL_CreateThread(thread_runner_soar, this);
	if (!soar_thread)
	{
		Broodwar->printf("Soar: Unable to create soar thread!");
		cout << "Soar: Unable to create soar thread!" << endl;
	}

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
{
	Unitset::iterator it;
	if ((it = minerals.find(unit)) != minerals.end())
	{
		delete_resource(unit->getID());
		return;
	}

	if ((it = vesp_gas.find(unit)) != vesp_gas.end())
	{
		delete_resource(unit->getID());
		return;
	}

	if ((it = my_units.find(unit)) != my_units.end())
	{
		delete_unit(unit->getID());
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

void Soar_Link::onUnitComplete(BWAPI::Unit *unit)
{

}

void Soar_Link::add_resource(int bw_id, int count, BWAPI::Position position, BWAPI::UnitType type)
{
	Identifier* input_link = agent->GetInputLink();

	Identifier* id;
	if (!input_link->FindByAttribute("resources", 0))
	{
		Broodwar->printf("WARNING: No 'units' identifier on the input link! Creating....");
		cout << "WARNING: No 'units' identifier on the input link! Creating...." << endl;

		id = input_link->CreateIdWME("resources");
	}
	else
		id = input_link->FindByAttribute("resources", 0)->ConvertToIdentifier();

	string name;

	if (type.getName().find("Mineral") != string::npos)
		name = "mineral";
	else
		name = "vesp-gas";

	Identifier* resource = id->CreateIdWME(name.c_str());

	resource->CreateIntWME("id", bw_id);
	resource->CreateIntWME("count", count);

	string svs_object_id = type.getName();
	svs_object_id.erase(remove_if(svs_object_id.begin(), svs_object_id.end(), isspace), svs_object_id.end());

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

	string svs_command = "a " + svs_object_id + " world v " + unit_box_verts + " p " + position_svs + " s " + size + " r 0 0 0";
	//Broodwar->printf("%s", svs_command.c_str());
	cout << svs_command << endl;

	SDL_mutexP(mu);
	agent->SendSVSInput(svs_command);
	SDL_mutexV(mu);

	resource->CreateStringWME("svsobject", svs_object_id.c_str());

	test_input_file << "I-resources-" << name << ": ^svsobject " << svs_object_id << endl;

	test_input_file << "SVS-Actual: " << svs_command << endl;
}

void Soar_Link::delete_resource(int bw_id)
{
	Identifier* input_link = agent->GetInputLink();

	Identifier* id;

	if (!input_link->FindByAttribute("resources", 0))
	{
		cout << "ERROR: No 'resources' identifier on the input link! Creating...." << endl;

		id = input_link->CreateIdWME("resources");
	}
	else
		id = input_link->FindByAttribute("resources", 0)->ConvertToIdentifier();

	for (int j = 0;j < id->GetNumberChildren();j++)
	{
		Identifier* unit;
		if (!id->GetChild(j)->IsIdentifier())
			continue;
		else
			unit = id->GetChild(j)->ConvertToIdentifier();

		if (unit->FindByAttribute("id", 0)->ConvertToIntElement()->GetValue() == bw_id)
		{
			string svs_object_id = unit->FindByAttribute("svsobject", 0)->ConvertToStringElement()->GetValue();

			string svs_command = "d " + svs_object_id;

			test_input_file << "SVS-Actual: " << svs_command << endl;

			event_queue.add_event(Soar_Event(svs_command, true));
			event_queue.add_event(Soar_Event(id->GetChild(j)));

			break;
		}
	}

	SDL_mutexP(mu);
	Unitset::iterator it;
	if ((it = minerals.find(getUnitFromID(bw_id))) != minerals.end())
		minerals.erase(it);
	else if ((it = vesp_gas.find(getUnitFromID(bw_id))) != vesp_gas.end())
		vesp_gas.erase(it);
	SDL_mutexV(mu);
}

void Soar_Link::update_resources()
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
}

void Soar_Link::delete_unit(int uid)
{
	Identifier* input_link = agent->GetInputLink();
	Identifier* id;

	if (!input_link->FindByAttribute("units", 0))
	{
		cout << "ERROR: No 'units' identifier on the input link! Creating...." << endl;

		id = input_link->CreateIdWME("units");
	}
	else
	{
		id = input_link->FindByAttribute("units", 0)->ConvertToIdentifier();

		for (int j = 0;j < id->GetNumberChildren();j++)
		{
			Identifier* unit;
			if (!id->GetChild(j)->IsIdentifier())
				continue;
			else
				unit = id->GetChild(j)->ConvertToIdentifier();

			if (unit->FindByAttribute("id", 0)->ConvertToIntElement()->GetValue() == uid)
			{
				string svs_object_id = unit->FindByAttribute("svsobject", 0)->ConvertToStringElement()->GetValue();

				string svs_command = "d " + svs_object_id;

				test_input_file << "SVS-Actual: " << svs_command << endl;

				event_queue.add_event(Soar_Event(svs_command, true));
				event_queue.add_event(Soar_Event(id->GetChild(j)));

				break;
			}
		}
	}
	
	SDL_mutexP(mu);
	my_units.erase(my_units.find(getUnitFromID(uid)));
	SDL_mutexV(mu);
}

void Soar_Link::update_units()
{
	Unitset my_units_new = Broodwar->self()->getUnits();

	Identifier* input_link = agent->GetInputLink();

	for (Unitset::iterator it = my_units_new.begin();it != my_units_new.end();it++)
	{
		if (my_units.find(*it) == my_units.end())
		{
			Unit* bw_unit = (*it);

			Identifier* id;
			if (!input_link->FindByAttribute("units", 0))
			{
				//Broodwar->printf("WARNING: No 'units' identifier on the input link! Creating....");
				cout << "WARNING: No 'units' identifier on the input link! Creating...." << endl;

				id = input_link->CreateIdWME("units");
			}
			else
				id = input_link->FindByAttribute("units", 0)->ConvertToIdentifier();

			Identifier* unit;

			if (!bw_unit->getType().isBuilding())
			{
				unit = id->CreateIdWME("unit");

				test_input_file << "I-units-unit:";

				unit->CreateIntWME("id", bw_unit->getID());
				test_input_file << " ^id " << bw_unit->getID();

				bool worker = bw_unit->getType().isWorker();
				unit->CreateIntWME("worker", worker);
				test_input_file << " ^worker " << worker;

				if (worker)
				{
					unit->CreateIntWME("idle", bw_unit->isIdle());
					test_input_file << " ^idle " << bw_unit->isIdle();

					unit->CreateIntWME("carrying", (bw_unit->isCarryingGas() || bw_unit->isCarryingMinerals() || bw_unit->getPowerUp()));
					test_input_file << " ^carring " << (bw_unit->isCarryingGas() || bw_unit->isCarryingMinerals() || bw_unit->getPowerUp());

					if (bw_unit->isCarryingGas())
					{
						unit->CreateStringWME("carrying", "gas");
						test_input_file << " ^carring gas";
					}
					else if (bw_unit->isCarryingMinerals())
					{
						unit->CreateStringWME("carring", "minerals");
						test_input_file << " ^carrying minerals";
					}
					else if (bw_unit->getPowerUp())
					{
						unit->CreateStringWME("carrying", "powerup");
						test_input_file << " ^carrying powerup";
					}
				}
				else
				{
					//TODO handle units other than workers
				}
			}
			else
			{
				unit = id->CreateIdWME("building");

				test_input_file << "I-units-building:";

				unit->CreateIntWME("id", bw_unit->getID());
				test_input_file << " ^id " << bw_unit->getID();
			}

			string svs_object_id = bw_unit->getType().getName();
			svs_object_id.erase(remove_if(svs_object_id.begin(), svs_object_id.end(), isspace), svs_object_id.end());

			stringstream ss;
			ss << bw_unit->getID();
			svs_object_id += ss.str();
			ss.str("");

			int size_y = bw_unit->getType().dimensionUp() + bw_unit->getType().dimensionDown() + 1;

			//Flip the point so "north" isn't negative y
			ss << ((float)bw_unit->getLeft()/32.0f) << " " << flip_one_d_point(((float)bw_unit->getTop() + size_y)/32.0f, false) << " 0";
			string position = ss.str();
			ss.str("");

			ss << ((float)(bw_unit->getType().dimensionLeft() + bw_unit->getType().dimensionRight() + 1))/32.0f << " " << ((float)(size_y))/32.0f << " 1";
			string size = ss.str();
			ss.str("");

			string svs_command = "a " + svs_object_id + " world v " + unit_box_verts + " p " + position + " s " + size + " r 0 0 0";
			//Broodwar->printf("%s", svs_command.c_str());
			cout << svs_command << endl;

			SDL_mutexP(mu);
			agent->SendSVSInput(svs_command);
			SDL_mutexV(mu);

			unit->CreateStringWME("svsobject", svs_object_id.c_str());

			test_input_file << " ^svsobject " << svs_object_id << endl;

			test_input_file << "SVS-Actual: " << svs_command << endl;

			my_units.insert((*it));
		}
		else
		{
			Unitset::iterator old_it = my_units.find(*it);

			Unit* new_unit = *it;
			Unit* old_unit = *old_it;

			int size_y = new_unit->getType().dimensionUp() + new_unit->getType().dimensionDown() + 1;

			stringstream ss;
			ss << ((float)new_unit->getLeft()/32.0f) << " " << flip_one_d_point(((float)new_unit->getTop() + size_y)/32.0f, false) << " 0";
			string position = ss.str();
			ss.str("");

			string svs_object_id = new_unit->getType().getName();
			svs_object_id.erase(remove_if(svs_object_id.begin(), svs_object_id.end(), isspace), svs_object_id.end());

			ss.str("");
			ss << new_unit->getID();
			svs_object_id += ss.str();
			ss.str("");

			string svs_command = "c " + svs_object_id + " p " + position;
			test_input_file << "SVS-Actual: " << svs_command << endl;

			SDL_mutexP(mu);
			agent->SendSVSInput(svs_command);
			SDL_mutexV(mu);

			if (!new_unit->getType().isBuilding())
			{
				Identifier* input_link = agent->GetInputLink();
				Identifier* units = input_link->FindByAttribute("units", 0)->ConvertToIdentifier();

				for (int i = 0;i < units->GetNumberChildren();i++)
				{
					Identifier* unit = units->GetChild(i)->ConvertToIdentifier();

					WMElement* id = unit->FindByAttribute("id", 0);
					IntElement* id_int = id->ConvertToIntElement();

					int unit_id = int(id_int->GetValue());
					int to_change_id = new_unit->getID();

					if (unit_id == to_change_id)
					{
						WMElement* idle = unit->FindByAttribute("idle", 0);
						IntElement* idle_int = idle->ConvertToIntElement();

						idle_int->Update(new_unit->isIdle());
						break;
					}
				}
			}
		}
	}
}

Unit* Soar_Link::getUnitFromID(string id_string)
{
	int id;
	stringstream ss(id_string);
	ss >> id;

	return getUnitFromID(id);
}

Unit* Soar_Link::getUnitFromID(int id)
{
	return Broodwar->getUnit(id);
}
