//Soar SC Headers
#include "Soar_SC.h"
#include "Soar_Link.h"
#include "BWAPI_Link.h"
#include "Terrain.h"
#include "Soar_Unit.h"

#include "Timer.h"

using namespace sml;
using namespace std;
using namespace BWAPI;

Soar_Link::Soar_Link(Soar_SC* soar_sc_link) //Constructor for the Soar Link class
{
	this->soar_sc_link = soar_sc_link;

	//kernel = Kernel::CreateKernelInCurrentThread(true); //Create a new Soar kernel (agent spawner) in a new thread
	//kernel->StopEventThread();
	
	//kernel = Kernel::CreateRemoteConnection(false, "127.0.0.1", 12121);
	kernel = Kernel::CreateKernelInNewThread();

	mu = SDL_CreateMutex(); //Create a new mutex to prevent simultanous access to shared objects

	kill_threads = false; //Make sure the initial soar agent run thread doesn't immediately die when it is created
	had_interrupt = false; //Make sure the misc handler will properly handle run starts and stops by setting the default value
}

Soar_Link::~Soar_Link() //Deconstructor
{
	kill_threads = true; //Tell the soar agent run thread to die if it is still running

	soar_sc_link->add_event(Soar_Event("stop-soar", false));

	SDL_WaitThread(soar_thread, nullptr); //Wait for the soar agent run thread to die if it's till running
	SDL_DestroyMutex(mu); //Then destroy the mutex

	kernel->DestroyAgent(agent); //Shut down the agent
	kernel->Shutdown(); //Then shut down the soar kernel

	for (size_t i = 0;i < fog_tiles.size();i++)
		delete fog_tiles[i];
}

void Soar_Link::output_handler(smlRunEventId id, void* d, Agent *a, smlPhase phase) //The after output phase handler
{
	Timer time;
	time.StartTimer();

#pragma omp atomic
	decisions++;

	int commands = a->GetNumberCommands();

	for (int i = 0;i < commands;i++) //Parse all the agent's commands
	{
		int j = 0;

		Identifier* output_command = a->GetCommand(i);

        string name  = output_command->GetCommandName();

		if (name == "move") //Move command
		{
			string object_to_move = output_command->GetParameterValue("object");
			Unit unit = soar_sc_link->get_bwapi_link()->getUnitFromID(object_to_move);

			WMElement* type_wme = output_command->FindByAttribute("type", 0);

			assert(type_wme != nullptr);

			StringElement* type_string = type_wme->ConvertToStringElement();

			if (string(type_string->GetValue()) == "svs-coordinates" ||
				string(type_string->GetValue()) == "svs-object")
			{
				//svs-coordinates
				//This move command is to move to the coordinates of the svs object (or try to)
				//But NOT to attack them or anything

				//svs-object
				//This move command is to move to the svs object by doing the equivelent of a *right click* on it
				//This will attack units for instance
				WMElement* svsobject_wme = output_command->FindByAttribute("svsobject", 0); //The SVS Object ID
				WMElement* position_wme = output_command->FindByAttribute("position", 0); //Whether to move the the upper left, center, etc. of the object

				string buffer;

				string svsobject_string = svsobject_wme->GetValueAsString(buffer);
				string position_string = position_wme->GetValueAsString(buffer);

				Soar_Unit* soar_unit = soar_unit_from_svsobject_id(svsobject_string);

				if (soar_unit == nullptr)
				{
					output_command->AddStatusError();
					continue;
				}

				BWAPI::Position pos;

				if (position_string == "center")
				{
					Soar_Unit::Position upper_left = soar_unit->lastPosition();
					Soar_Unit::Size size = soar_unit->get_size();

					pos.x = int(upper_left.x) + int(size.x/2.0f);
					pos.y = int(upper_left.y) + int(size.y/2.0f);
				}
				else if (position_string == "upper_left")
				{
					Soar_Unit::Position upper_left = soar_unit->lastPosition();

					pos.x = int(upper_left.x);
					pos.y = int(upper_left.y);
				}
				else if (position_string == "upper_right")
				{
					Soar_Unit::Position upper_left = soar_unit->lastPosition();
					Soar_Unit::Size size = soar_unit->get_size();

					pos.x = int(upper_left.x + size.x);
					pos.y = int(upper_left.y);
				}
				else if (position_string == "lower_left")
				{
					Soar_Unit::Position upper_left = soar_unit->lastPosition();
					Soar_Unit::Size size = soar_unit->get_size();

					pos.x = int(upper_left.x);
					pos.y = int(upper_left.y + size.y);
				}
				else if (position_string == "lower_right")
				{
					Soar_Unit::Position upper_left = soar_unit->lastPosition();
					Soar_Unit::Size size = soar_unit->get_size();
					
					pos.x = int(upper_left.x + size.x);
					pos.y = int(upper_left.y + size.y);
				}
				else
				{
					output_command->AddStatusError();
					continue;
				}

				pos.y = int(Terrain::flip_one_d_point(float(pos.y), false))*32;
				pos.x *= 32;
				
				if (string(type_string->GetValue()) == "svs-coordinates")
					soar_sc_link->add_event(BWAPI_Event(UnitCommand::move(unit, pos), output_command, soar_sc_link));
				else
					soar_sc_link->add_event(BWAPI_Event(UnitCommand::rightClick(unit, pos), output_command, soar_sc_link));
			}
			else if (string(type_string->GetValue()) == "coordinates")
			{
				//This move command is to move to the coordinates specified
				//It's given in x and y
				//Not a right click!
				WMElement* x_wme = output_command->FindByAttribute("x", 0);
				WMElement* y_wme = output_command->FindByAttribute("y", 0);

				string buffer;

				string x_string = x_wme->GetValueAsString(buffer);
				string y_string = y_wme->GetValueAsString(buffer);

				int x;
				int y;

				stringstream ss(x_string);
				ss >> x;

				ss.str("");
				ss.clear();

				ss.str(y_string);
				ss >> y;

				y = int(Terrain::flip_one_d_point(float(y), false))*32;
				x *= 32;

				soar_sc_link->add_event(BWAPI_Event(UnitCommand::move(unit, BWAPI::Position(x,y)), output_command, soar_sc_link));
			}
			else if (string(type_string->GetValue()) == "bwapi-object")
			{
				//This move command is to move to the bwapi-object by performing a right click
				//This will attack the object for instance
				WMElement* bwapi_object_wme = output_command->FindByAttribute("bwapi-object", 0);

				string buffer;

				string id_string = bwapi_object_wme->GetValueAsString(buffer);

				stringstream ss(id_string);

				int id_int;
				ss >> id_int;

				BWAPI::Unit target = soar_sc_link->get_bwapi_link()->getUnitFromID(id_int);

				soar_sc_link->add_event(BWAPI_Event(UnitCommand::rightClick(unit,target), output_command, soar_sc_link));
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

			//Flip the y axis around
			y = int(Terrain::flip_one_d_point(float(y), false));

			Unit worker = soar_sc_link->get_bwapi_link()->getUnitFromID(worker_id);

			if (!worker->isIdle())
				soar_sc_link->add_event(BWAPI_Event(UnitCommand::stop(worker), nullptr, soar_sc_link));

			soar_sc_link->add_event(BWAPI_Event(UnitCommand::build(worker, TilePosition(x,y), unit_type), output_command, soar_sc_link));
		}
		else if (name == "build-unit")
		{
			string type = output_command->GetParameterValue("type");

			stringstream ss(type);
			int type_id;

			ss >> type_id;

			UnitType unit_type(type_id);

			string location = output_command->GetParameterValue("building");

			Unit unit_location = soar_sc_link->get_bwapi_link()->getUnitFromID(location);

			soar_sc_link->add_event(BWAPI_Event(UnitCommand::train(unit_location, unit_type), output_command, soar_sc_link));
		}
	}

	//Update the units and resources
	SDL_mutexP(mu);

	soar_sc_link->get_bwapi_link()->update_units();

	soar_sc_link->get_bwapi_link()->update_resources();

	update_fogOfWar(0.0f, 0.0f, float(Broodwar->mapWidth()), float(Broodwar->mapHeight()));

	soar_sc_link->soar_thread_update(); //Then have the events in the queue execute

	SDL_mutexV(mu);

	double seconds = time.GetTime();
	cout << "Time (0): " << seconds << endl;
	
	if (seconds > 0.02)
		Broodwar->printf("Time: %f", seconds);

	cout << "Time: " << seconds << endl;
}

void Soar_Link::print_soar(smlPrintEventId id, void *d, Agent *a, char const *m) //Print handler, handles all output of the agent
{}

void Soar_Link::misc_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase) //Handler for handling misc stuff, at this point handles run starts and stops
{
	switch (id)
	{
		case smlEVENT_AFTER_RUN_ENDS:
		{
			if (!had_interrupt)
			{
				soar_sc_link->set_should_run_forever(true);
				soar_sc_link->signal_soar_updates();
				had_interrupt = true;
			}
			break;
		}
		case smlEVENT_BEFORE_RUN_STARTS:
		{
			if (had_interrupt)
			{
				soar_sc_link->set_should_run_forever(false);
				had_interrupt = false;
			}
			break;
		}
		default:
			break;
	}
}

void Soar_Link::start_soar_run()
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
	agent->SetOutputLinkChangeTracking(false);

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

	//agent->RegisterForPrintEvent(smlEVENT_PRINT, printcb, this); //Register for the print event to handle all output from the agent and other misc info like which thing is running etc.
	
	agent->ExecuteCommandLine("watch 0"); //Not strictly needed but for being verbose, watch 1 outputs all the decision stuff like what the agent is doing along with outputs via write etc.
	agent->ExecuteCommandLine("source Soar-SC/Soar-SC.soar"); //Load our agent's source into memory

	send_base_input(agent, false);

	//Create the initial soar runner thread
	soar_thread = SDL_CreateThread(thread_runner_soar, this);
	if (!soar_thread) //Check to make sure the thread exists
	{ //It doesn't so tell the user
		Broodwar->printf("Soar: Unable to create soar thread!");
		cout << "Soar: Unable to create soar thread!" << endl;
	}
}

void Soar_Link::update_fogOfWar(float x_start, float y_start, float size_x, float size_y)
{
	static vector<pair<int, int> > visible_tiles;

	Identifier* input_link = agent->GetInputLink();
	WMElement* elem = input_link->FindByAttribute("fog-tiles", 0);
	assert(elem == nullptr);

	Identifier* fog_tiles = elem->ConvertToIdentifier();

	for (size_t x = size_t(x_start);x < size_x+x_start;x++)
	{
		for (size_t y = size_t(y_start);y < size_y+y_start;y++)
		{
			//output.push_back(make_pair("Fog-Time (1): ", time.GetTime()));

			int id_x = x - x % 4;
			int id_y = size_t(Terrain::flip_one_d_point(float(y - y % 4), false));
			pair<int, int> block = make_pair(id_x, id_y);

			//output.push_back(make_pair("Fog-Time (2): ", time.GetTime()));

			if (find(visible_tiles.begin(), visible_tiles.end(), block) != visible_tiles.end())
			{
				//output.push_back(make_pair("Fog-Time (3-1): ", time.GetTime()));
				continue;
			}

			//output.push_back(make_pair("Fog-Time (3-2): ", time.GetTime()));

			if (Broodwar->isExplored(x, y) || Broodwar->isVisible(x,y))
			{
				std::stringstream ss_x;
				ss_x << id_x;
				std::stringstream ss_y;
				ss_y << id_y;

				WMElement* tile_wme = fog_tiles->FindByAttribute(string("fog-tile-" + ss_x.str() + "-" + ss_y.str()).c_str(), 0);

				if (tile_wme == nullptr)
					continue;

				Identifier* child = tile_wme->ConvertToIdentifier();

				soar_sc_link->add_event(Soar_Event(child));

				std::string svsobject_id = "BaseFogTile:" + ss_x.str() + ":" + ss_y.str();

				std::string command = "d " + svsobject_id;

				soar_sc_link->add_event(Soar_Event(command, true));

				for (vector<Soar_Unit*>::iterator it = this->fog_tiles.begin();it != this->fog_tiles.end();it++)
				{
					if ((*it)->get_svsobject_id() == svsobject_id)
					{
						delete *it;
						this->fog_tiles.erase(it);
						break; //Remove the Soar Unit*
					}
				}

				visible_tiles.push_back(block);
			}
		}
	}
}

void Soar_Link::send_base_input(Agent* agent, bool wait_for_analyzer)
{
	//Load all the types of the Starcraft onto the input link for making the agent's life easier
	UnitType::set types = UnitTypes::allUnitTypes(); //Get them all

	Identifier* input_link = agent->GetInputLink(); //Grab the input link

	WMElement* my_race_wme = get_child("my_race", input_link);

	if (my_race_wme == nullptr)
		my_race_wme = input_link->CreateStringWME("my_race", "unknown");

	StringElement* my_race_str = my_race_wme->ConvertToStringElement();

	assert(my_race_str != nullptr);

	my_race_str->Update(BWAPI::Broodwar->self()->getRace().getName().c_str());

	Identifier* types_id = nullptr; //Create a variable for holding the types Identifier
	if (!input_link->FindByAttribute("types", 0)) //Check if there is a types Identifier, at this point there shouldn't be but it's fine (somewhat) if there is
		types_id = input_link->CreateIdWME("types")->ConvertToIdentifier(); //It doesn't exist so create it
	else //Otherwise
		types_id = input_link->FindByAttribute("types", 0)->ConvertToIdentifier(); //Grab the existing one

	//Loop throug all the units and add them to the input link
	for (UnitType::set::iterator it = types.begin();it != types.end();it++)
	{
		char* name = const_cast<char*>((*it).getName().c_str());
		//replace(&name[0], &name[strlen(name)], '_', ' ');

		Identifier* type = types_id->CreateIdWME("type")->ConvertToIdentifier(); //Create a new type Identifier on the types Identifier
		type->CreateStringWME("name", name); //Create a string WME with the type's name
		type->CreateIntWME("id", (*it).getID()); //Create an Int WME with the type's unique ID
		type->CreateIntWME("mineral-cost", (*it).mineralPrice());
		type->CreateIntWME("gas-cost", (*it).gasPrice());
		type->CreateIntWME("x-size", (*it).tileWidth());
		type->CreateIntWME("y-size", (*it).tileHeight());
		type->CreateIntWME("building", (*it).isBuilding());
		type->CreateIntWME("my_count", 0);
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
	string svs_command_1 = "a TerrainBorder1 terrain_object world v " + Terrain::unit_box_verts + " p 0 -1 0 s " + map_width_as_string + " 1 1";

	agent->SendSVSInput(svs_command_1); //Send the SVS command to the agent

	//Top barrier
	string svs_command_2 = "a TerrainBorder3 terrain_object world v " + Terrain::unit_box_verts + " p 0 " + map_height_as_string + " 0 s " + map_width_as_string + " 1 1";

	agent->SendSVSInput(svs_command_2);

	//Left barrier
	string svs_command_3 = "a TerrainBorder4 terrain_object world v " + Terrain::unit_box_verts + " p -1 0 0 s 1 " + map_height_as_string + " 1";

	agent->SendSVSInput(svs_command_3);

	//Right barrier
	string svs_command_4 = "a TerrainBorder2 terrain_object world v " + Terrain::unit_box_verts + " p " + map_width_as_string + " 0 0 s 1 " + map_height_as_string + " 1";

	agent->SendSVSInput(svs_command_4);

	//Send the corners
	string corner_north_west = "a TerrainCornerNW terrain_object world v " + Terrain::unit_box_verts + " p 1 1 0 s 1 1 1";
	string corner_south_west = "a TerrainCornerSW terrain_object world v " + Terrain::unit_box_verts + " p 1 " + map_height_as_string + " 0 s 1 1 1";
	string corner_north_east = "a TerrainCornerNE terrain_object world v " + Terrain::unit_box_verts + " p " + map_width_as_string + " 1 0 s 1 1 1";
	string corner_south_east = "a TerrainCornerSE terrain_object world v " + Terrain::unit_box_verts + " p " + map_width_as_string + " " + map_height_as_string + " 0 s 1 1 1";

	agent->SendSVSInput(corner_north_west);
	agent->SendSVSInput(corner_south_west);
	agent->SendSVSInput(corner_north_east);
	agent->SendSVSInput(corner_south_east);

	Identifier* corners_id = nullptr;

	if (!input_link->FindByAttribute("map-corners", 0)) //Check if there is a types Identifier, at this point there shouldn't be but it's fine (somewhat) if there is
		corners_id = input_link->CreateIdWME("map-corners")->ConvertToIdentifier(); //It doesn't exist so create it
	else //Otherwise
		corners_id = input_link->FindByAttribute("map-corners", 0)->ConvertToIdentifier(); //Grab the existing one

	{
		Identifier* corner = corners_id->CreateIdWME("corner");
		corner->CreateStringWME("svsobject", "TerrainCornerNW");
		
		Soar_Unit* soar_tile = new Soar_Unit(soar_sc_link, nullptr, false);
		Soar_Unit::Position temp;

		temp.x = 0.0f;
		temp.y = Terrain::flip_one_d_point(0.0f, false);

		soar_tile->set_position(temp);

		temp.x = 2.0f;
		temp.y = 2.0f;

		soar_tile->set_size(temp);
		soar_tile->set_svsobject_id("TerrainCornerNW"); //REMEMBER TO DO THIS LAST NOT FIRST.  IF YOU SET IT FIRST NOTHING ELSE LIKE POSITION WILL ACTUALLY BE SET

		terrain_corners.push_back(soar_tile);
	}

	{
		Identifier* corner = corners_id->CreateIdWME("corner");
		corner->CreateStringWME("svsobject", "TerrainCornerSW");

		Soar_Unit* soar_tile = new Soar_Unit(soar_sc_link, nullptr, false);
		Soar_Unit::Position temp;

		temp.x = 0.0f;
		temp.y = Terrain::flip_one_d_point(float(Broodwar->mapHeight()-1), false);

		soar_tile->set_position(temp);

		temp.x = 2.0f;
		temp.y = 2.0f;

		soar_tile->set_size(temp);
		soar_tile->set_svsobject_id("TerrainCornerSW"); //REMEMBER TO DO THIS LAST NOT FIRST.  IF YOU SET IT FIRST NOTHING ELSE LIKE POSITION WILL ACTUALLY BE SET

		terrain_corners.push_back(soar_tile);
	}

	{
		Identifier* corner = corners_id->CreateIdWME("corner");
		corner->CreateStringWME("svsobject", "TerrainCornerNE");

		Soar_Unit* soar_tile = new Soar_Unit(soar_sc_link, nullptr, false);
		Soar_Unit::Position temp;

		temp.x = float(Broodwar->mapWidth()-1);
		temp.y = Terrain::flip_one_d_point(0.0f, false);

		soar_tile->set_position(temp);

		temp.x = 2.0f;
		temp.y = 2.0f;

		soar_tile->set_size(temp);
		soar_tile->set_svsobject_id("TerrainCornerNE"); //REMEMBER TO DO THIS LAST NOT FIRST.  IF YOU SET IT FIRST NOTHING ELSE LIKE POSITION WILL ACTUALLY BE SET

		terrain_corners.push_back(soar_tile);
	}

	{
		Identifier* corner = corners_id->CreateIdWME("corner");
		corner->CreateStringWME("svsobject", "TerrainCornerSE");

		Soar_Unit* soar_tile = new Soar_Unit(soar_sc_link, nullptr, false);
		Soar_Unit::Position temp;

		temp.x = float(Broodwar->mapWidth()-1);
		temp.y = Terrain::flip_one_d_point(float(Broodwar->mapHeight()-1), false);

		soar_tile->set_position(temp);

		temp.x = 2.0f;
		temp.y = 2.0f;

		soar_tile->set_size(temp);
		soar_tile->set_svsobject_id("TerrainCornerSE"); //REMEMBER TO DO THIS LAST NOT FIRST.  IF YOU SET IT FIRST NOTHING ELSE LIKE POSITION WILL ACTUALLY BE SET

		terrain_corners.push_back(soar_tile);
	}

	vector<vector<bool> > initial_base_map; //Variable for containing the map and whether a tile is walkable or not
	size_t map_size_x = Broodwar->mapWidth() * 4; //Set the size to the number of walkable tiles, build tiles times 4
	size_t map_size_y = Broodwar->mapHeight() * 4; //Same thing as above

	string named_map_names = "A";
	int named_map_pos = 0;
	vector<Terrain::named_point_map> free_tiles;

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
			if (initial_base_map[y][x] && !Terrain::vector_named_map_contains_point(free_tiles, make_pair<int,int>(x,y)))
			{
				Terrain::named_point_map new_free_section;

				Terrain::flood_fill(make_pair<int,int>(x,y), initial_base_map, true, new_free_section);

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
	
	for (int i = 0;i < int(initial_base_map.size()*4);i++)
	{
		vector<bool> temporary_vector;
		temporary_vector.reserve(initial_base_map[int(floor(float(i)/4))].size()*4);

		for (int j = 0;j < int(initial_base_map[int(floor(float(i)/4))].size()*4);j++)
			temporary_vector.push_back(true);
		
		map.push_back(temporary_vector);
	}

	//Scale the map back up while making a new version of it
	for (int y = 0;y < int(initial_base_map.size()*4);y++)
	{
		for (int x = 0;x < int(initial_base_map[int(floor(float(y)/4))].size()*4);x++)
		{
			if (!Terrain::vector_named_map_contains_point(free_tiles, make_pair<int,int>(int(floor(float(x)/4)),int(floor(float(y)/4)))))
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
			ss_y << size_t(Terrain::flip_one_d_point(float(y), false));

			std::string svsobject_id = "BaseFogTile:" + ss_x.str() + ":" + ss_y.str();

			std::string svs_command = "a " + svsobject_id + " fog_tile world v " + Terrain::unit_box_verts + " p " + ss_x.str() + " " + ss_y.str() + " 10 s 4 4 1";
			agent->SendSVSInput(svs_command);

			Identifier* fogTile = fog_tiles->CreateIdWME(("fog-tile-" + ss_x.str() + "-" + ss_y.str()).c_str())->ConvertToIdentifier(); //Create a new type Identifier on the types Identifier
			fogTile->CreateStringWME("svsobject", svsobject_id.c_str()); //Create a string WME with the type's name

			//Create a Soar_Unit container for the tile
			Soar_Unit* soar_tile = new Soar_Unit(soar_sc_link, nullptr, false);
			Soar_Unit::Position temp;

			temp.x = float(x);
			temp.y = Terrain::flip_one_d_point(float(y), false);

			soar_tile->set_position(temp);

			temp.x = 4.0f;
			temp.y = 4.0f;

			soar_tile->set_size(temp);
			soar_tile->set_svsobject_id(svsobject_id); //REMEMBER TO DO THIS LAST NOT FIRST.  IF YOU SET IT FIRST NOTHING ELSE LIKE POSITION WILL ACTUALLY BE SET

			this->fog_tiles.push_back(soar_tile);
		}
	}

	//Create a new terrain analyzer
	terrain = new Terrain(map, agent);
	terrain->analyze(); //Start analyzing the terrain

	soar_sc_link->get_bwapi_link()->update_units();
	soar_sc_link->get_bwapi_link()->update_resources();
	update_fogOfWar(0.0f, 0.0f, float(Broodwar->mapWidth()), float(Broodwar->mapHeight()));

	cout << "Soar-SC is running." << endl; //Tell the user everything is working perfectly so far
	Broodwar->printf("Soar-SC is running.");
}

void Soar_Link::SendSVSInput(std::string input)
{
	agent->SendSVSInput(input);
}

void Soar_Link::ExecuteCommandLine(std::string input)
{
	agent->ExecuteCommandLine(input.c_str());
}

sml::Identifier* Soar_Link::GetOutputLink()
{
	return agent->GetOutputLink();
}

sml::Identifier* Soar_Link::GetInputLink()
{
	return agent->GetInputLink();
}

void Soar_Link::add_resource(int bw_id, int count, BWAPI::Position position, BWAPI::UnitType type)
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

	string svs_object_id = "Resource" + string(name == "mineral" ? "Mineral" : "Vesp"); //Set the svs id to be the type's name
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
	string svs_command = "a " + svs_object_id + " resource world v " + Terrain::unit_box_verts + " p " + position_svs + " s " + size + " r 0 0 0";
	//Broodwar->printf("%s", svs_command.c_str());
	cout << svs_command << endl;

	//Send the svs command to the agent
	soar_sc_link->get_soar_link()->SendSVSInput(svs_command);

	//Create the svs object id on the input link
	resource->CreateStringWME("svsobject", svs_object_id.c_str());
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

			soar_sc_link->add_event(Soar_Event(svs_command, true)); //Add the svs command to the queue
			soar_sc_link->add_event(Soar_Event(id->GetChild(j))); //And add the wme to destroy to the queue

			break; //Break because we're done
		}
	}

	soar_sc_link->get_bwapi_link()->delete_resource(bw_id); //Remove the middleware links
}

void Soar_Link::update_resource_count(int minerals, int gas, int total_supplies, int used_supplies)
{
	Identifier* input_link = agent->GetInputLink();
	
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

int Soar_Link::soar_agent_thread() //Thread for initial run of the soar agent
{
	SetThreadName("Soar Run Thread", GetCurrentThreadId());

	soar_sc_link->get_bwapi_link()->update_units();
	soar_sc_link->get_bwapi_link()->update_resources();
	
	agent->RunSelfForever();

	return 0;
}

int Soar_Link::get_decisions()
{
	return decisions;
}

void Soar_Link::set_decisions(int new_count)
{
#pragma omp atomic
	decisions = new_count;
}

Soar_Unit* Soar_Link::soar_unit_from_svsobject_id(std::string svsobject_id)
{
	//Slight optimization/hack
	if (strncmp(svsobject_id.c_str(), "BaseFogTile:", 12) == 0)
	{
		for (size_t i = 0;i < fog_tiles.size();i++)
		{
			if (fog_tiles[i]->get_svsobject_id() == svsobject_id)
				return fog_tiles[i];
		}
	}
	else if (strncmp(svsobject_id.c_str(), "TerrainCorner", 13) == 0)
	{
		for (size_t i = 0;i < terrain_corners.size();i++)
		{
			if (terrain_corners[i]->get_svsobject_id() == svsobject_id)
				return terrain_corners[i];
		}
	}
	else
	{
		map<BWAPI::Unit, Soar_Unit*> units = soar_sc_link->get_bwapi_link()->get_units();

		for (map<BWAPI::Unit, Soar_Unit*>::iterator it = units.begin();it != units.end();it++)
		{
			Soar_Unit* unit = it->second;

			if (unit->get_svsobject_id() == svsobject_id)
				return unit;
		}
	}

	return nullptr;
}

void Soar_Link::update_update_unit_count(std::map<BWAPI::UnitType, unsigned int> unit_counts)
{
	Identifier* input_link = agent->GetInputLink();
	WMElement* types_wme = input_link->FindByAttribute("types", 0);

	if (types_wme == nullptr)
		types_wme = input_link->CreateIdWME("types");

	Identifier* types = types_wme->ConvertToIdentifier();

	for (map<BWAPI::UnitType, unsigned int>::iterator it = unit_counts.begin();it != unit_counts.end();it++)
	{
		sml::WMElement* type_wme = find_child_with_attribute_value("type", "name", it->first.getName(), types);

		assert(type_wme != nullptr); //Corrupt memory?

		sml::Identifier* type_id = type_wme->ConvertToIdentifier();

		assert(type_id != nullptr); //Should always be an identifier

		sml::WMElement* my_count_wme = get_child("my_count", type_id);

		if (my_count_wme == nullptr)
			my_count_wme = type_id->CreateIntWME("my_count", 0);

		sml::IntElement* my_count_int = my_count_wme->ConvertToIntElement();
		
		assert(my_count_int != nullptr);

		if (it->second != int(my_count_int->GetValue())) //Prevent "blinking"
			my_count_int->Update(it->second);
	}
}

sml::WMElement* Soar_Link::get_child(std::string name, sml::Identifier* parent)
{
	for (sml::Identifier::ChildrenIter it = parent->GetChildrenBegin();it != parent->GetChildrenEnd();it++)
	{
		if ((*it)->GetAttribute() == name)
			return *it;
	}

	return nullptr;
}

sml::WMElement* Soar_Link::find_child_with_attribute_value(std::string attribute_name, std::string value, sml::Identifier* parent)
{
	return find_child_with_attribute_value("", attribute_name, value, parent);
}

sml::WMElement* Soar_Link::find_child_with_attribute_value(std::string child_name, std::string attribute_name, std::string value, sml::Identifier* parent)
{
	for (sml::Identifier::ChildrenIter it = parent->GetChildrenBegin();it != parent->GetChildrenEnd();it++)
	{
		if ((*it)->GetAttribute() == child_name || child_name == "")
		{
			sml::Identifier* child = (*it)->ConvertToIdentifier();

			if (child != nullptr)
			{
				sml::WMElement* attribute = get_child(attribute_name, child);

				if (attribute != nullptr)
				{
					string buffer;

					string result = attribute->GetValueAsString(buffer);

					if (result == value)
						return child;
				}
			}
		}
	}

	return nullptr;
}
