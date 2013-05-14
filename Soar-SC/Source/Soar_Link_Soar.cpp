#include "Soar_Link.h" //Include the Soar Link class header

#include <set> //for std::set
#include <vector> //For std::vector
#include <sstream> //For std::stringstream
#include <stack>

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
	//Timer timer;
	//timer.StartTimer();

	int commands = a->GetNumberCommands();

	//cout << "Time (0): " << timer.GetTimeMiliseconds() << endl;

	for (int i = 0;i < commands;i++) //Parse all the agent's commands
	{
		int j = 0;

		//cout << "Time (1-0): " << timer.GetTimeMiliseconds() << endl;

		Identifier* output_command = a->GetCommand(i);

        string name  = output_command->GetCommandName();

		//cout << "Time (1-1): " << timer.GetTimeMiliseconds() << endl;

		if (name == "move") //Move command
		{
			string object_to_move = output_command->GetParameterValue("object");
			Unit* unit = getUnitFromID(object_to_move);

			bool action = true;

			string destination;
			Unit* dest;

			float x;
			float y;

			WMElement* destination_wme = output_command->FindByAttribute("dest", 0);
			WMElement* location_string_wme = output_command->FindByAttribute("location-string", 0);
			if (destination_wme == NULL && location_string_wme == NULL)
			{
				IntElement* x_wme = output_command->FindByAttribute("x", 0)->ConvertToIntElement();
				IntElement* y_wme = output_command->FindByAttribute("y", 0)->ConvertToIntElement();

				x = float(x_wme->GetValue());
				y = float(y_wme->GetValue());

				cout << "Moving " << unit->getType().getName() << " to X: " << x << " " << "Y: " << y << " from " << unit->getPosition().x << " " << flip_one_d_point(unit->getPosition().y, false) << endl;

				action = false;
			}
			else if (destination_wme == NULL && location_string_wme != NULL)
			{
				string location = location_string_wme->ConvertToStringElement()->GetValue();

				cout << "Fog Tile Location: " << location << endl;

				int x_start = location.find(':', 0)+1;
				int x_end = location.find(':', x_start);
				int y_start = x_end+1;
				int y_end = location.size();

				string x_cord = location.substr(x_start, x_end-x_start);
				string y_cord = location.substr(y_start, y_end-y_start);

				cout << "Moving " << unit->getType().getName() << " to X: " << x_cord << " " << "Y: " << y_cord << " from " << unit->getPosition().x/32.0f << " " << flip_one_d_point(unit->getPosition().y, false)/32.0f << endl;

				x = 0;
				y = 0;

				stringstream ss;
				ss << x_cord;
				ss >> x;

				stringstream ss2;

				ss2 << y_cord;
				ss2 >> y;

				action = false;
			}
			else
			{
				destination = output_command->GetParameterValue("dest");
				dest = getUnitFromID(destination);
			}

			y = flip_one_d_point(y, false)*32.0f;
			x *= 32.0f;

			cout << "Moving to final position (" << x << "," << y << ") from (" << unit->getPosition().x << "," << unit->getPosition().y << ")" << endl;
			
			if (action)
			{
				if (unit != NULL && dest != NULL)
					event_queue.add_event(BWAPI_Event(UnitCommand::rightClick(unit, dest), output_command, this));
				else
					output_command->AddStatusError();
			}
			else
				event_queue.add_event(BWAPI_Event(UnitCommand::rightClick(unit, Position(x,y)), output_command, this));

			//cout << "Time (1-2-0): " << timer.GetTimeMiliseconds() << endl;
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

			//cout << "Time (1-2-1): " << timer.GetTimeMiliseconds() << endl;
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

			//cout << "Time (1-2-2): " << timer.GetTimeMiliseconds() << endl;

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

	//cout << "Time (1-3): " << timer.GetTimeMiliseconds() << endl;

	//Update the units and resources
	SDL_mutexP(mu);

	//cout << "Time (1-4): " << timer.GetTimeMiliseconds() << endl;

	update_units();

	//cout << "Time (1-5): " << timer.GetTimeMiliseconds() << endl;

	update_resources();

	update_fogOfWar(0.0f, 0.0f, float(Broodwar->mapWidth()), float(Broodwar->mapHeight()));

	//cout << "Time (1-7): " << timer.GetTimeMiliseconds() << endl;

	event_queue.update(); //Then have the events in the queue execute

	//cout << "Time (1-8): " << timer.GetTimeMiliseconds() << endl;

	SDL_mutexV(mu);

	test_input_file << "--------------------------------------------------" << endl;

	//cout << "Total Time: " << timer.GetTimeMiliseconds() << endl;
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

//typedef vector<pair<int,int> > map_array;
//typedef pair<string, map_array> named_map;

bool Soar_Link::named_map_contains_point(named_map &map, pair<int,int> &point)
{
	map_array* internal_map = &(map.second);
	int internal_map_size = internal_map->size();

	for (int i = 0;i < internal_map_size;i++)
	{
		if (point.first == internal_map->at(i).first &&
			point.second == internal_map->at(i).second)
			return true;
	}

	return false;
}

bool Soar_Link::vector_named_map_contains_point(vector<named_map> &vector_map, pair<int,int> &point)
{
	int vector_map_size = vector_map.size();

	for (int i = 0;i < vector_map_size;i++)
	{
		if (named_map_contains_point(vector_map[i], point))
			return true;
	}

	return false;
}

void Soar_Link::flood_fill(std::pair<int,int> &location, std::vector<std::vector<bool> > &bool_map, bool target, named_map &fill_vector)
{
	stack<pair<int,int> > locations;

	if (location.second > bool_map.size() || location.first > bool_map[location.second].size() || location.first < 0 || location.second < 0)
		return;

	if (bool_map[location.second][location.first] != target)
		return;

	if (named_map_contains_point(fill_vector, location))
		return;

	locations.push(location);

	while (!locations.empty())
	{
		pair<int,int> n = locations.top();
		locations.pop();

		if (n.second > bool_map.size()				||
			n.first > bool_map[n.second].size()		||
			n.first < 0								||
			n.second < 0							||
			bool_map[n.second][n.first] != target	||
			named_map_contains_point(fill_vector, n))
			continue;

		pair<int,int> west,east;
		west = n;
		east = n;
		west.first--;
		east.first++;

		if (west.first >= 0)
		{
			while (	west.first > 0 &&
					bool_map[west.second][west.first] == target &&
					!named_map_contains_point(fill_vector, west))
			{
				fill_vector.second.push_back(west);
				west.first--;
			}
		}
		else
			west.first = 0;

		if (east.first < bool_map[n.second].size())
		{
			while (	east.first < bool_map[n.second].size() &&
					bool_map[east.second][east.first] == target &&
					!named_map_contains_point(fill_vector, east))
			{
				fill_vector.second.push_back(east);
				east.first++;
			}
		}
		else
			east.first = bool_map[n.second].size()-1;

		fill_vector.second.push_back(n);

		for (int x = west.first;x < east.first;x++)
		{
			if ((n.second+1) < bool_map.size() &&
				bool_map[n.second+1][x] == target)
				locations.push(make_pair<int,int>(x,n.second+1));
			
			if ((n.second-1) >= 0 &&
				bool_map[n.second-1][x] == target)
				locations.push(make_pair<int,int>(x,n.second-1));
		}
	}

	//Done
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
		char* name = const_cast<char*>((*it).getName().c_str());
		replace(&name[0], &name[strlen(name)], '_', ' ');

		Identifier* type = types_id->CreateIdWME("type")->ConvertToIdentifier(); //Create a new type Identifier on the types Identifier
		type->CreateStringWME("name", name); //Create a string WME with the type's name
		type->CreateIntWME("id", (*it).getID()); //Create an Int WME with the type's unique ID
		type->CreateIntWME("mineral-cost", (*it).mineralPrice());
		type->CreateIntWME("gas-cost", (*it).gasPrice());
		type->CreateIntWME("building", (*it).isBuilding());
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

	vector<vector<bool> > initial_base_map; //Variable for containing the map and whether a tile is walkable or not
	size_t map_size_x = Broodwar->mapWidth() * 4; //Set the size to the number of walkable tiles, build tiles times 4
	size_t map_size_y = Broodwar->mapHeight() * 4; //Same thing as above

	string named_map_names = "A";
	int named_map_pos = 0;
	vector<named_map> free_tiles;

	//Value for determining whether to keep named_maps of less than X size
	const int min_size_cap = 5;

	for (int y = 0;y < int(map_size_y);y += 4) //Loop through every tile in the map and map the tile to a boolean in the vector vector of booleans.
	{
		vector<bool> map_y;

		for (int x = 0;x < int(map_size_x);x += 4)
		{
			bool walkable[4][4];
			int is_walkable_count = 0;

			for (int i = 0;i < 4;i++)
			{
				for (int j = 0;j < 4;j++)
				{
					walkable[i][j] = Broodwar->isWalkable(x+i,y+j);
					if (walkable[i][j])
						is_walkable_count++;
				}
			}

			if (is_walkable_count > 12)
				map_y.push_back(true);
			else
				map_y.push_back(false);
		}

		initial_base_map.push_back(map_y);
	}

	for (int y = 0;y < int(initial_base_map.size());y++)
	{
		for (int x = 0;x < int(initial_base_map[y].size());x++)
		{
			if (initial_base_map[y][x] && !vector_named_map_contains_point(free_tiles, make_pair<int,int>(x,y)))
			{
				named_map new_free_section;

				flood_fill(make_pair<int,int>(x,y), initial_base_map, true, new_free_section);

				if (new_free_section.second.size() < min_size_cap)
					continue;

				new_free_section.first = named_map_names;

				named_map_names[named_map_pos]++;

				if (named_map_names[named_map_pos] > 'Z')
				{
					named_map_pos++;
					named_map_names.push_back('A');
				}

				free_tiles.push_back(new_free_section);
			}
		}
	}

	vector<vector<bool> > map;
	map.reserve(initial_base_map.size()*4);
	
	for (int i = 0;i < initial_base_map.size()*4;i++)
	{
		vector<bool> temporary_vector;
		temporary_vector.reserve(initial_base_map[floor(float(i)/4)].size()*4);

		for (int j = 0;j < initial_base_map[floor(float(i)/4)].size()*4;j++)
			temporary_vector.push_back(true);
		
		map.push_back(temporary_vector);
	}

	//Scale the map back up while making a new version of it
	for (int y = 0;y < int(initial_base_map.size()*4);y++)
	{
		for (int x = 0;x < int(initial_base_map[floor(float(y)/4)].size()*4);x++)
		{
			if (!vector_named_map_contains_point(free_tiles, make_pair<int,int>(floor(float(x)/4),floor(float(y)/4))))
				map[y][x] = false;
			else
				map[y][x] = true;
		}
	}

	size_t fogOfWarSize_x = Broodwar->mapWidth();
	size_t fogOfWarSize_y = Broodwar->mapHeight();

	Identifier* fog_tiles = input_link->CreateIdWME("fog-tiles")->ConvertToIdentifier();

	for (size_t x = 0;x < fogOfWarSize_x;x += 4)
	{
		for (size_t y = 0;y < fogOfWarSize_y;y += 4)
		{
			std::stringstream ss_x;
			std::stringstream ss_y;
			ss_x << x;
			ss_y << size_t(flip_one_d_point(float(y), false));

			std::string svsobject_id = "fog:" + ss_x.str() + ":" + ss_y.str();

			std::string svs_command = "a " + svsobject_id + " world v " + unit_box_verts + " p " + ss_x.str() + " " + ss_y.str() + " 10 s 4 4 1";
			agent->SendSVSInput(svs_command);

			Identifier* fogTile = fog_tiles->CreateIdWME(string(ss_x.str() + ":" + ss_y.str()).c_str())->ConvertToIdentifier(); //Create a new type Identifier on the types Identifier
			fogTile->CreateStringWME("svsobject", svsobject_id.c_str()); //Create a string WME with the type's name
		}
	}

	//Create a new terrain analyzer
	analyzer = new TerrainAnalyzer(map, agent, mu);
	analyzer->analyze(); //Start analyzing the terrain

	update_units();
	update_resources();
	update_fogOfWar(0.0f, 0.0f, float(Broodwar->mapWidth()), float(Broodwar->mapHeight()));

	if (wait_for_analyzer)
	{
		while(!analyzer->done_sending_svs())
			Sleep(10);
	}
}
