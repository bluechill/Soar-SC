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

const std::string Soar_Link::unit_box_verts = "0 0 0 0 0 1 0 1 0 0 1 1 1 0 0 1 0 1 1 1 0 1 1 1";

Soar_Link::Soar_Link()
	: cout_redirect("bwapi-data/logs/stdout.txt"),
	cerr_redirect("bwapi-data/logs/stderr.txt")
{
	if (!cout_redirect || !cerr_redirect)
		Broodwar->printf("Unable to redirect output!");

	cout_orig_buffer = cout.rdbuf();
	cout.rdbuf(cout_redirect.rdbuf());

	cerr_orig_buffer = cerr.rdbuf();
	cerr.rdbuf(cerr_redirect.rdbuf());

	kernel = Kernel::CreateKernelInNewThread();
	//kernel = Kernel::CreateRemoteConnection(false, "35.0.136.73", 12121);

	mutex = CreateMutex(NULL, FALSE, NULL);
	if (!mutex)
	{
		cout << "Unable to create mutex." << endl;
		ExitProcess(7);
	}

	done_updating = false;
}

Soar_Link::~Soar_Link()
{
	/*if (WaitForSingleObject(thread_handle, 1000) != WAIT_OBJECT_0)
		TerminateThread(thread_handle, 3);*/

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

	cout << "Soar-SC is running." << endl;
	Broodwar->printf("Soar-SC is running.");

	//const char* result = agent->ExecuteCommandLine("print s1");
	//cout << "Soar: " << result << endl;
	//Broodwar->printf("Soar: %s", result);

	//Uncomment for dealing with barriers within Soar
	/*thread_handle = CreateThread(NULL, 0, thread_runner, (void*) this, 0, &thread_id);

	if (!thread_handle)
	{
	cout << "Unable to create thread!" << endl;
	ExitProcess(3);
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

		agent->SendSVSInput(svs_string);
	}

	cout << "Time: " << time(NULL) << endl;

	cout << "Done updating map" << endl;

	DWORD result = WaitForSingleObject(mutex, INFINITE);

	switch (result)
	{
	case WAIT_OBJECT_0:
		{
			done_updating = true;

			if (!ReleaseMutex(mutex))
			{
				cout << "Unable to release mutex" << endl;
				ExitProcess(9);
			}
			break;
		}

	case WAIT_ABANDONED:
		{
			cout << "Abandoned after infinte time! Error on mutex!" << endl;
			ExitProcess(9999);
			break;
		}
	}

	cout << "Done!" << endl;
}

void Soar_Link::update_resources()
{

}

void Soar_Link::update_units()
{
	Unitset my_units_new = Broodwar->self()->getUnits();

	for (Unitset::iterator it = my_units_new.begin();it != my_units_new.end();it++)
	{
		if (my_units.find(*it) == my_units.end())
		{
			Unit* bw_unit = (*it);

			if (!bw_unit->getType().isBuilding())
			{
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

				Identifier* unit = id->CreateIdWME("unit");
				unit->CreateIntWME("id", bw_unit->getID());
				bool worker = bw_unit->getType().isWorker();
				unit->CreateIntWME("worker", worker);

				if (worker)
				{
					unit->CreateIntWME("idle", bw_unit->isIdle());
					unit->CreateIntWME("carrying", (bw_unit->isCarryingGas() || bw_unit->isCarryingMinerals() || bw_unit->getPowerUp()));

					if (bw_unit->isCarryingGas())
						unit->CreateStringWME("carrying", "gas");
					else if (bw_unit->isCarryingMinerals())
						unit->CreateStringWME("carring", "minerals");
					else if (bw_unit->getPowerUp())
						unit->CreateStringWME("carrying", "powerup");
				}
				else
				{
					//TODO handle units other than workers
				}

				string svs_object_id = bw_unit->getType().getName();
				svs_object_id.erase(remove_if(svs_object_id.begin(), svs_object_id.end(), isspace), svs_object_id.end());

				stringstream ss;
				ss << bw_unit->getID();
				svs_object_id += ss.str();
				ss.str("");
				ss << bw_unit->getPosition().x/4 << " " << bw_unit->getPosition().y/4 << " 0";
				string position = ss.str();
				ss.str("");
				
				ss << bw_unit->getType().tileWidth() << " " << bw_unit->getType().tileHeight() << " 1";
				string size = ss.str();
				ss.str("");

				ss << bw_unit->getAngle();
				string rotation = ss.str();

				string svs_command = "a " + svs_object_id + " world v " + unit_box_verts + " p " + position + " s " + size + " r 0 " + rotation + " 0";
				Broodwar->printf("%s", svs_command.c_str());
				cout << svs_command << endl;

				agent->SendSVSInput(svs_command);
				
				unit->CreateStringWME("svsobject", svs_object_id.c_str());
			}
		}
	}

	Unitset final_units;

	for (Unitset::iterator it = my_units_new.begin(), it_next = it;it != my_units_new.end();it++)
	{
		Identifier* input_link = agent->GetInputLink();

		if (!(*it)->exists() || (*it)->getType().isBuilding())
		{
			Identifier* id;
			if (!input_link->FindByAttribute("units", 0))
			{
				cout << "ERROR: No 'units' identifier on the input link! Creating...." << endl;

				id = input_link->CreateIdWME("units");
			}
			else
				id = input_link->FindByAttribute("units", 0)->ConvertToIdentifier();

			if (id->GetParameterValue("name") == "units")
			{
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

						agent->SendSVSInput(svs_command);

						id->GetChild(j)->DestroyWME();

						break;
					}
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
	/*DWORD result = WaitForSingleObject(mutex, 10);

	switch (result)
	{
	case WAIT_OBJECT_0:
	{
	__try
	{
	if (!done_updating)
	return;
	}

	__finally
	{
	if (!ReleaseMutex(mutex))
	{
	cout << "Unable to release mutex" << endl;
	ExitProcess(9);
	}
	}
	}

	case WAIT_ABANDONED:
	{
	cout << "Abandoned after 10 miliseconds (mutex)" << endl;
	return;
	}
	}*/

	update_units();
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
