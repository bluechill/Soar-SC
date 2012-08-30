#include "Soar_Link.h" //Include the Soar Link class header

#include <set> //for std::set
#include <vector> //For std::vector
#include <sstream> //For std::stringstream

#include <windows.h> //For windows related functions

#include <time.h>

#include "SDL/SDL.h"

using namespace BWAPI; //Use namespaces to allow the use of string instead of std::string for example
using namespace std;
using namespace sml;

int Soar_Link::soar_agent_thread() //Thread for initial run of the soar agent
{
	//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	SetThreadName("Soar Run", GetCurrentThreadId());

	update_units();
	update_resources();

	while(!analyzer->done_sending_svs())
		Sleep(10);

	//test_input_file << "--------------------------------------------------" << endl;

	agent->RunSelfForever();

	return 0;
}

void Soar_Link::output_handler(smlRunEventId id, void* d, Agent *a, smlPhase phase) //The after output phase handler
{
	int commands = a->GetNumberCommands();

	for (int i = 0;i < commands;i++) //Parse all the agent's commands
	{
		int j = 0;

		Identifier* output_command = a->GetCommand(i);

        string name  = output_command->GetCommandName();

		if (name == "move") //Move command
		{
			string object_to_move = output_command->GetParameterValue("object");
			string destination = output_command->GetParameterValue("dest");

			Unit* unit = getUnitFromID(object_to_move);
			Unit* dest = getUnitFromID(destination);

			if (unit != NULL && dest != NULL)
			{
				event_queue.add_event(BWAPI_Event(UnitCommand::rightClick(unit, dest), output_command, this));

				//if (!unit->rightClick(dest)) //Execute move command in starcraft
				//{
				//	Error e = Broodwar->getLastError();
				//	cerr << "Error (BWAPI) (RightClick-" << dest->getID() << "): " << e.toString() << endl;

				//	output_command->AddStatusError();
				//}
				//else
				//	output_command->AddStatusComplete();
			}
			else
				output_command->AddStatusError();
		}
		else if (name == "build-building") //Build command
		{
			string type = output_command->GetParameterValue("type");
				
			stringstream ss(type);
			int type_id;

			ss >> type_id;

			UnitType unit_type(type_id);

			string location_x = output_command->GetParameterValue("location-x");
			string location_y = output_command->GetParameterValue("location-y");

			string worker_id = output_command->GetParameterValue("worker");

			int x = 0;
			int y = 0;

			stringstream l_x(location_x);
			l_x >> x;
			stringstream l_y(location_y);
			l_y >> y;

			Unit* worker = getUnitFromID(worker_id);

			if (!worker->isIdle())
				event_queue.add_event(BWAPI_Event(UnitCommand::stop(worker), NULL, this));

			event_queue.add_event(BWAPI_Event(UnitCommand::build(worker, TilePosition(x,y), unit_type), output_command, this));
		}
		else if (name == "build-unit")
		{
			string type = output_command->GetParameterValue("type");

			stringstream ss(type);
			int type_id;

			ss >> type_id;

			UnitType unit_type(type_id);

			string location = output_command->GetParameterValue("building");

			Unit* unit_location = getUnitFromID(location);

			event_queue.add_event(BWAPI_Event(UnitCommand::train(unit_location, unit_type), output_command, this));

			/*if (!unit_location->train(unit_type))
			{
				Error e = Broodwar->getLastError();
				cerr << "Error (BWAPI) (Build Unit: " << unit_type.getName() << "): " << e.toString() << endl;

				output_command->AddStatusError();
			}
			else
				output_command->AddStatusComplete();*/
		}
	}

	//Update the units and resources
	SDL_mutexP(mu);

	update_units();

	update_resources();

	event_queue.update(); //Then have the events in the queue execute

	SDL_mutexV(mu);

	test_input_file << "--------------------------------------------------" << endl;
}

void Soar_Link::print_soar(smlPrintEventId id, void *d, Agent *a, char const *m) //Print handler, handles all output of the agent
{
	string output(m);
	if (console != NULL)
		console->recieve_input(output);
}

void Soar_Link::misc_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase) //Handler for handling misc stuff, at this point handles run starts and stops
{
	switch (id)
	{
		case smlEVENT_AFTER_RUN_ENDS:
		{
			if (!had_interrupt)
			{
				event_queue.update_forever();
				had_interrupt = true;
			}
			break;
		}
		case smlEVENT_BEFORE_RUN_STARTS:
		{
			if (had_interrupt)
			{
				event_queue.update_only_on_calls();
				had_interrupt = false;
			}
			break;
		}
		default:
			break;
	}
}

void Soar_Link::send_base_input(Agent* agent, bool wait_for_analyzer)
{
	//Load all the types of the Starcraft onto the input link for making the agent's life easier
	UnitType::set types = UnitTypes::allUnitTypes(); //Get them all

	Identifier* input_link = agent->GetInputLink(); //Grab the input link
	Identifier* types_id; //Create a variable for holding the types Identifier
	if (!input_link->FindByAttribute("types", 0)) //Check if there is a types Identifier, at this point there shouldn't be but it's fine (somewhat) if there is
		types_id = input_link->CreateIdWME("types")->ConvertToIdentifier(); //It doesn't exist so create it
	else //Otherwise
		types_id = input_link->FindByAttribute("types", 0)->ConvertToIdentifier(); //Grab the existing one

	//Loop throug all the units and add them to the input link
	for (UnitType::set::iterator it = types.begin();it != types.end();it++)
	{
		Identifier* type = types_id->CreateIdWME("type")->ConvertToIdentifier(); //Create a new type Identifier on the types Identifier
		type->CreateStringWME("name", (*it).getName().c_str()); //Create a string WME with the type's name
		type->CreateIntWME("id", (*it).getID()); //Create an Int WME with the type's unique ID
	}

	//Terrain stuff, Put the entire terrain into SVS including barriers around the map so the agent doesn't think it can place stuff there
	stringstream ss; //Create a string stream variable for converting the width and height of the map
	ss << Broodwar->mapWidth(); //Get the map width an put it in a string stream to convert it to a string
	string map_width_as_string = ss.str(); //Get the string representation
	ss.str(""); //Clear the string stream
	ss << Broodwar->mapHeight(); //Get the map height and put it in the string stream
	string map_height_as_string = ss.str(); //Get the string representation
	ss.str(""); //Clear the string stream once more

	//Bottom barrier
	string svs_command_1 = "a -x0 world v " + unit_box_verts + " p 0 -1 0 s " + map_width_as_string + " 1 1";

	test_input_file << "SVS-Actual: " << svs_command_1 << endl; //Output the svs command to the test input

	agent->SendSVSInput(svs_command_1); //Send the SVS command to the agent

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

	cout << "Soar-SC is running." << endl; //Tell the user everything is working perfectly so far
	Broodwar->printf("Soar-SC is running.");

	vector<vector<bool> > map; //Variable for containing the map and whether a tile is walkable or not
	size_t map_size_x = Broodwar->mapWidth() * 4; //Set the size to the number of walkable tiles, build tiles times 4
	size_t map_size_y = Broodwar->mapHeight() * 4; //Same thing as above

	vector<vector<bool> > build_tiles_map; //Low res buildabilty map

	for (int y = 0;y < int(map_size_y);y++) //Loop through every tile in the map and map the tile to a boolean in the vector vector of booleans.
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

	//Create a new terrain analyzer
	analyzer = new TerrainAnalyzer(map, agent, mu);
	analyzer->analyze(); //Start analyzing the terrain

	update_units();
	update_resources();

	if (wait_for_analyzer)
	{
		while(!analyzer->done_sending_svs())
			Sleep(10);
	}
}
