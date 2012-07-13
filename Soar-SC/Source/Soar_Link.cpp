#include "Soar_Link.h"

#include <set>
#include <vector>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace BWAPI;
using namespace sml;
using namespace std;

//Threading stuff
int thread_runner_soar(void* link)
{
	return reinterpret_cast<Soar_Link*>(link)->soar_agent_thread();
}

const std::string Soar_Link::unit_box_verts = "0 0 0 0 0 1 0 1 0 0 1 1 1 0 0 1 0 1 1 1 0 1 1 1";

float Soar_Link::flip_one_d_point(const float &point, const bool &x_axis)
{
	int map;

	if (x_axis)
		map = Broodwar->mapWidth();
	else
		map = Broodwar->mapHeight();

	float flipped_point = point;
	//Convert the point to be bewtween 0 and 1
	flipped_point /= map;
	//Subtract 0.5 so 0.5 in the point becomes 0
	flipped_point -= 0.5f;
	//Multiply by -1 to flip it around the axis
	flipped_point *= -1;
	//Add 0.5 again so it's from 0 to 1 again
	flipped_point += 0.5f;
	//Multiple by the map size in the axis to convert back to the point
	flipped_point *= map;

	//Return it
	return flipped_point;
}

Soar_Link::Soar_Link()
	: cout_redirect("bwapi-data/logs/stdout.txt"),
	cerr_redirect("bwapi-data/logs/stderr.txt"),
	test_input_file("bwapi-data/logs/test_input.txt")
{
	if (!cout_redirect || !cerr_redirect)
		Broodwar->printf("Unable to redirect output!");

	cout_orig_buffer = cout.rdbuf();
	cout.rdbuf(cout_redirect.rdbuf());

	cerr_orig_buffer = cerr.rdbuf();
	cerr.rdbuf(cerr_redirect.rdbuf());

	kernel = Kernel::CreateKernelInNewThread();
	//kernel = Kernel::CreateRemoteConnection(false, "35.0.136.73", 12121);

	mu = SDL_CreateMutex();

	done_updating = false;
	done_updating_agent = true;

	should_die = false;
}

Soar_Link::~Soar_Link()
{
	should_die = true;

	SDL_WaitThread(soar_thread, NULL);
	SDL_DestroyMutex(mu);

	cout.rdbuf(cout_orig_buffer);
	cerr.rdbuf(cerr_orig_buffer);

	kernel->DestroyAgent(agent);
	kernel->Shutdown();
}

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

	stringstream ss;
	ss << Broodwar->mapWidth()+1;
	string map_width_as_string = ss.str();
	ss.str("");
	ss << Broodwar->mapHeight()+1;
	string map_height_as_string = ss.str();
	ss.str("");

	//Bottom barrier
	string svs_command_1 = "a -x0 world v " + unit_box_verts + " p 0 -1 0 s 1 " + map_height_as_string + " 1";

	test_input_file << "SVS-Actual: " << svs_command_1 << endl;

	agent->SendSVSInput(svs_command_1);

	//Top barrier
	string svs_command_2 = "a x0 world v " + unit_box_verts + " p 0 " + map_height_as_string + " 0 s 1 " + map_height_as_string + " 1";

	test_input_file << "SVS-Actual: " << svs_command_2 << endl;

	agent->SendSVSInput(svs_command_2);

	//Left barrier
	string svs_command_3 = "a -y0 world v " + unit_box_verts + " p -1 0 0 s " + map_width_as_string + " 1 1";

	test_input_file << "SVS-Actual: " << svs_command_3 << endl;

	agent->SendSVSInput(svs_command_3);

	//Right barrier
	string svs_command_4 = "a y0 world v " + unit_box_verts + " p " + map_width_as_string + " 0 0 s " + map_width_as_string + " 1 1";

	test_input_file << "SVS-Actual: " << svs_command_4 << endl;

	agent->SendSVSInput(svs_command_4);

	cout << "Soar-SC is running." << endl;
	Broodwar->printf("Soar-SC is running.");

	soar_thread = SDL_CreateThread(thread_runner_soar, this);
	if (!soar_thread)
	{
		Broodwar->printf("Soar: Unable to create soar thread!");
		cout << "Soar: Unable to create soar thread!" << endl;
	}
}

void Soar_Link::onEnd(bool isWinner)
{
	// Called when the game ends
	if ( isWinner )
	{
		// Log your win here!
	}
}

void Soar_Link::update_map()
{
	set<vector<string> > polygons;

	vector<vector<bool> > walkable;

	cout << "Map width: " << Broodwar->mapWidth() << endl << "Map Height: " << Broodwar->mapHeight() << endl;

	cout << "Time: " << time(NULL) << endl;

	for (int x = 0;x < Broodwar->mapWidth()*4;x++)
	{
		vector<bool> y_array;

		for (int y = 0;y < Broodwar->mapHeight()*4;y++)
			y_array.push_back(Broodwar->isWalkable(x,y));

		walkable.push_back(y_array);
	}

	cout << "Walkable: " << walkable.size() << " " << walkable[0].size() << endl;

	//TODO: combine polygons
	for (unsigned int x = 0;x < walkable.size();x++)
	{
		for (unsigned int y = 0;y < walkable[x].size();y++)
		{
			if (walkable[x][y])
				continue;

			stringstream ss;
			ss << x << " " << y << " 1";
			string vertex1 = ss.str();
			ss.str("");

			ss << x + 1 << " " << y << " 1";
			string vertex2 = ss.str();
			ss.str("");

			ss << x + 1 << " " << y + 1 << " 1";
			string vertex3 = ss.str();
			ss.str("");

			ss << x << " " << y + 1 << " 1";
			string vertex4 = ss.str();
			ss.str("");

			ss << x << " " << y << " 0";
			string vertex5 = ss.str();
			ss.str("");

			ss << x + 1 << " " << y << " 0";
			string vertex6 = ss.str();
			ss.str("");

			ss << x + 1 << " " << y + 1 << " 0";
			string vertex7 = ss.str();
			ss.str("");

			ss << x << " " << y + 1 << " 0";
			string vertex8 = ss.str();
			ss.str("");

			vector<string> polygon;
			polygon.push_back(vertex1);
			polygon.push_back(vertex2);
			polygon.push_back(vertex3);
			polygon.push_back(vertex4);
			polygon.push_back(vertex5);
			polygon.push_back(vertex6);
			polygon.push_back(vertex7);
			polygon.push_back(vertex8);

			polygons.insert(polygon);
		}
	}

	cout << "Done creating polygons: " << polygons.size() << endl;

	for (set<vector<string> >::iterator it = polygons.begin();it != polygons.end();it++)
	{
		string svs_string = "a ";
		svs_string += "barrier";

		stringstream ss;
		ss << (*it)[0][0] << (*it)[1][0];
		svs_string += ss.str();
		ss.str("");

		svs_string += " world v ";

		for (vector<string>::const_iterator p_it = it->begin();p_it != it->end();p_it++)
		{
			svs_string += (*p_it);
			svs_string += " ";
		}

		svs_string += "p 0 0 0";

		test_input_file << "SVS-Actual: " << svs_string << endl;

		agent->SendSVSInput(svs_string);
	}

	cout << "Done updating map" << endl;
}

void Soar_Link::add_resource(int bw_id, int count, BWAPI::Position position, BWAPI::UnitType type, float angle)
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

	ss << ((float)type.dimensionLeft())*2.0f/32.0f << " " << ((float)type.dimensionUp())*2.0f/32.0f << " 1";
	string size = ss.str();
	ss.str("");


	ss << angle;
	string rotation = ss.str();

	string svs_command = "a " + svs_object_id + " world v " + unit_box_verts + " p " + position_svs + " s " + size + " r 0 " + rotation + " 0";
	//Broodwar->printf("%s", svs_command.c_str());
	cout << svs_command << endl;

	agent->SendSVSInput(svs_command);

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

			agent->SendSVSInput(svs_command);

			id->GetChild(j)->DestroyWME();

			break;
		}
	}
}

void Soar_Link::update_resources()
{
	Unitset visible_minerals = Broodwar->getMinerals();
	Unitset visible_vesp_gas = Broodwar->getGeysers();

	Unitset new_minerals;
	Unitset new_vesp_gas;

	for (Unitset::iterator it = visible_minerals.begin();it != visible_minerals.end();it++)
	{
		if (minerals.find(*it) == minerals.end())
		{
			//Doesn't exist in my current list of visible minerals

			Unit* bw_unit = (*it);

			add_resource(bw_unit->getID(), bw_unit->getResources(), bw_unit->getPosition(), bw_unit->getType(), (float)bw_unit->getAngle());
		}
		
		new_minerals.push_back(*it);
	}

	for (Unitset::iterator it = visible_vesp_gas.begin();it != visible_vesp_gas.end();it++)
	{
		if (vesp_gas.find(*it) == vesp_gas.end())
		{
			//Doesn't exist in my current list of visible vespian gas

			Unit* bw_unit = (*it);

			add_resource(bw_unit->getID(), bw_unit->getResources(), bw_unit->getPosition(), bw_unit->getType(), (float)bw_unit->getAngle());
		}

		new_vesp_gas.push_back(*it);
	}

	Unitset final_minerals;

	for (Unitset::iterator it = new_minerals.begin();it != new_minerals.end();it++)
	{
		if (!(*it)->exists())
			delete_resource((*it)->getID());
		else
			final_minerals.insert(*it);
	}

	Unitset final_vesp_gas;

	for (Unitset::iterator it = new_vesp_gas.begin();it != new_vesp_gas.end();it++)
	{
		if (!(*it)->exists())
			delete_resource((*it)->getID());
		else
			final_vesp_gas.insert(*it);
	}

	minerals = final_minerals;
	vesp_gas = final_vesp_gas;
}

void Soar_Link::update_units()
{
	Unitset my_units_new = Broodwar->self()->getUnits();

	for (Unitset::iterator it = my_units_new.begin();it != my_units_new.end();it++)
	{
		if (my_units.find(*it) == my_units.end())
		{
			Unit* bw_unit = (*it);

			Identifier* input_link = agent->GetInputLink();

			Identifier* id;
			if (!input_link->FindByAttribute("units", 0))
			{
				Broodwar->printf("WARNING: No 'units' identifier on the input link! Creating....");
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
			//Flip the point so "north" isn't negative y
			ss << ((float)bw_unit->getLeft()/32.0f) << " " << flip_one_d_point(((float)bw_unit->getTop())/32.0f, false) << " 0";
			string position = ss.str();
			ss.str("");

			ss << ((float)bw_unit->getType().dimensionLeft())*2.0f/32.0f << " " << ((float)bw_unit->getType().dimensionUp())*2.0f/32.0f << " 1";
			string size = ss.str();
			ss.str("");

			ss << bw_unit->getAngle();
			string rotation = ss.str();

			string svs_command = "a " + svs_object_id + " world v " + unit_box_verts + " p " + position + " s " + size + " r 0 " + rotation + " 0";
			Broodwar->printf("%s", svs_command.c_str());
			cout << svs_command << endl;

			agent->SendSVSInput(svs_command);

			unit->CreateStringWME("svsobject", svs_object_id.c_str());

			test_input_file << " ^svsobject " << svs_object_id << endl;

			test_input_file << "SVS-Actual: " << svs_command << endl;
		}
		else
		{
			Unit* orig = *my_units.find(*it);
			Unit* new_unit = *it;

			string svs_object_id = new_unit->getType().getName();
			svs_object_id.erase(remove_if(svs_object_id.begin(), svs_object_id.end(), isspace), svs_object_id.end());

			stringstream ss;
			ss << new_unit->getID();
			svs_object_id += ss.str();
			ss.str("");

			string svs_command = "c " + svs_object_id;

			if (orig->getLeft() != new_unit->getLeft() ||
				orig->getTop() != new_unit->getTop())
			{
				ss << ((float)new_unit->getLeft()/32.0f) << " " << flip_one_d_point(((float)new_unit->getTop())/32.0f, false) << " 0";
				string position = ss.str();
				ss.str("");

				svs_command += " " + position;
			}

			if (orig->getAngle() != new_unit->getAngle())
			{
				ss << new_unit->getAngle();
				string rotation = ss.str();

				svs_command += " 0 " + rotation + " 0";
			}

			if (svs_command != ("c " + svs_object_id))
			{
				agent->SendSVSInput(svs_command);
				test_input_file << "SVSAction: " << svs_command << endl;
			}
		}

		my_units_new.insert(*it);
	}

	Unitset final_units;

	for (Unitset::iterator it = my_units_new.begin(), it_next = it;it != my_units_new.end();it++)
	{
		Identifier* input_link = agent->GetInputLink();

		if (!(*it)->exists())
		{
			Identifier* id;

			if (!input_link->FindByAttribute("units", 0))
			{
				cout << "ERROR: No 'units' identifier on the input link! Creating...." << endl;

				id = input_link->CreateIdWME("units");
			}
			else
				id = input_link->FindByAttribute("units", 0)->ConvertToIdentifier();

				for (int j = 0;j < id->GetNumberChildren();j++)
				{
					Identifier* unit;
					if (!id->GetChild(j)->IsIdentifier())
						continue;
					else
						unit = id->GetChild(j)->ConvertToIdentifier();

					if (unit->FindByAttribute("id", 0)->ConvertToIntElement()->GetValue() == (*it)->getID())
					{
						string svs_object_id = unit->FindByAttribute("svsobject", 0)->ConvertToStringElement()->GetValue();

						string svs_command = "d " + svs_object_id;

						test_input_file << "SVS-Actual: " << svs_command << endl;

						agent->SendSVSInput(svs_command);

						id->GetChild(j)->DestroyWME();

						break;
					}
				}
		}
		else
			final_units.insert(*it);
	}

	my_units = final_units;
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

	//Uncomment for dealing with barriers within Soar

	SDL_mutexP(mu);
	if (done_updating_agent)
		done_updating_agent = false;
	SDL_mutexV(mu);
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

int Soar_Link::soar_agent_thread()
{
	while (!should_die)
	{
		SDL_mutexP(mu);
		if (done_updating_agent)
		{
			SDL_mutexV(mu);
			Sleep(10);
			continue;
		}
		SDL_mutexV(mu);

		update_units();
		update_resources();

		agent->ExecuteCommandLine("run -d 1");

		SDL_mutexP(mu);
		done_updating_agent = true;
		SDL_mutexV(mu);
	}

	return 0;
}
