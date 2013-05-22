//Soar SC Headers
#include "Soar_SC.h"
#include "Soar_Link.h"
#include "BWAPI_Link.h"
#include "Terrain.h"
#include "Soar_Unit.h"

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

	soar_sc_link->add_event(Soar_Event("halt", false)); //Execute a stop command to stop the agent

	SDL_WaitThread(soar_thread, nullptr); //Wait for the soar agent run thread to die if it's till running
	SDL_DestroyMutex(mu); //Then destroy the mutex

	kernel->DestroyAgent(agent); //Shut down the agent
	kernel->Shutdown(); //Then shut down the soar kernel
}

void Soar_Link::output_handler(smlRunEventId id, void* d, Agent *a, smlPhase phase) //The after output phase handler
{
	/*kernel->CheckForIncomingCommands();
	kernel->CheckForIncomingEvents();*/
	

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
			Unit* unit = soar_sc_link->get_bwapi_link()->getUnitFromID(object_to_move);

			bool action = true;

			string destination;
			Unit* dest;

			float x;
			float y;

			WMElement* destination_wme = output_command->FindByAttribute("dest", 0);
			WMElement* location_string_wme = output_command->FindByAttribute("location-string", 0);
			if (destination_wme == nullptr && location_string_wme == nullptr)
			{
				IntElement* x_wme = output_command->FindByAttribute("x", 0)->ConvertToIntElement();
				IntElement* y_wme = output_command->FindByAttribute("y", 0)->ConvertToIntElement();

				if (x_wme != NULL)
					x = float(x_wme->GetValue());
				else
				{
					StringElement* x_wme_string = output_command->FindByAttribute("x", 0)->ConvertToStringElement();

					assert (x_wme_string != NULL);

					stringstream ss(x_wme_string->GetValue());
					int value;
					ss >> value;

					x = float(value);
				}

				if (y_wme != NULL)
					y = float(y_wme->GetValue());
				else
				{
					StringElement* y_wme_string = output_command->FindByAttribute("y", 0)->ConvertToStringElement();

					assert (y_wme_string != NULL);

					stringstream ss(y_wme_string->GetValue());
					int value;
					ss >> value;

					y = float(value);
				}


				cout << "Moving " << unit->getType().getName() << " to X: " << x << " " << "Y: " << y << " from " << unit->getPosition().x << " " << Terrain::flip_one_d_point(float(unit->getPosition().y), false) << endl;

				action = false;
			}
			else if (destination_wme == nullptr && location_string_wme != nullptr)
			{
				string location = location_string_wme->ConvertToStringElement()->GetValue();

				cout << "Fog Tile Location: " << location << endl;

				int x_start = location.find(':', 0)+1;
				int x_end = location.find(':', x_start);
				int y_start = x_end+1;
				int y_end = location.size();

				string x_cord = location.substr(x_start, x_end-x_start);
				string y_cord = location.substr(y_start, y_end-y_start);

				cout << "Moving " << unit->getType().getName() << " to X: " << x_cord << " " << "Y: " << y_cord << " from " << float(unit->getPosition().x)/32.0f << " " << Terrain::flip_one_d_point(float(unit->getPosition().y), false)/32.0f << endl;

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
				dest = soar_sc_link->get_bwapi_link()->getUnitFromID(destination);
			}

			y = Terrain::flip_one_d_point(y, false)*32.0f;
			x *= 32.0f;

			cout << "Moving to final position (" << x << "," << y << ") from (" << unit->getPosition().x << "," << unit->getPosition().y << ")" << endl;
			
			if (action)
			{
				if (unit != nullptr && dest != nullptr)
					soar_sc_link->add_event(BWAPI_Event(UnitCommand::rightClick(unit, dest), output_command, soar_sc_link));
				else
					output_command->AddStatusError();
			}
			else
				soar_sc_link->add_event(BWAPI_Event(UnitCommand::rightClick(unit, Position(int(x),int(y))), output_command, soar_sc_link));
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

			Unit* worker = soar_sc_link->get_bwapi_link()->getUnitFromID(worker_id);

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

			Unit* unit_location = soar_sc_link->get_bwapi_link()->getUnitFromID(location);

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
			int id_x = x - x % 4;
			int id_y = size_t(Terrain::flip_one_d_point(float(y - y % 4), false));
			pair<int, int> block = make_pair(id_x, id_y);

			if (find(visible_tiles.begin(), visible_tiles.end(), block) != visible_tiles.end())
				continue;

			if (Broodwar->isExplored(x, y) || Broodwar->isVisible(x,y))
			{
				std::stringstream ss_x;
				ss_x << id_x;
				std::stringstream ss_y;
				ss_y << id_y;

				WMElement* tile_wme = fog_tiles->FindByAttribute(string(ss_x.str() + ":" + ss_y.str()).c_str(), 0);

				if (tile_wme == nullptr)
					continue; //Ignore it, probably should but oh well, TODO: fix ME!

				Identifier* child = tile_wme->ConvertToIdentifier();

				soar_sc_link->add_event(Soar_Event(child));

				std::string svsobject_id = "BaseFogTile:" + ss_x.str() + ":" + ss_y.str();

				std::string command = "d " + svsobject_id;

				soar_sc_link->add_event(Soar_Event(command, true));

				visible_tiles.push_back(block);
				sort(visible_tiles.begin(), visible_tiles.end());
			}
		}
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

	cout << "Soar-SC is running." << endl; //Tell the user everything is working perfectly so far
	Broodwar->printf("Soar-SC is running.");

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

			Identifier* fogTile = fog_tiles->CreateIdWME(string(ss_x.str() + ":" + ss_y.str()).c_str())->ConvertToIdentifier(); //Create a new type Identifier on the types Identifier
			fogTile->CreateStringWME("svsobject", svsobject_id.c_str()); //Create a string WME with the type's name
		}
	}

	//Create a new terrain analyzer
	terrain = new Terrain(map, agent);
	terrain->analyze(); //Start analyzing the terrain

	soar_sc_link->get_bwapi_link()->update_units();
	soar_sc_link->get_bwapi_link()->update_resources();
	update_fogOfWar(0.0f, 0.0f, float(Broodwar->mapWidth()), float(Broodwar->mapHeight()));
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
