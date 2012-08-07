#include "Soar_Link.h" //Include the Soar Link class header

#include <set> //for std::set
#include <vector> //For std::vector
#include <sstream> //For std::stringstream

#include <windows.h> //For windows related functions

#include <ctime>

using namespace BWAPI; //Use namespaces to allow the use of string instead of std::string for example
using namespace std;
using namespace sml;

int Soar_Link::soar_agent_thread() //Thread for initial run of the soar agent
{
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
	clock_t time_start = clock();
	clock_t time_end;

	int commands = a->GetNumberCommands();

	for (int i = 0;i < commands;i++) //Parse all the agent's commands
	{
		Identifier* output_command = a->GetCommand(i);

        string name  = output_command->GetCommandName();

		if (name == "move") //Move command
		{
			string object_to_move = output_command->GetParameterValue("object");
			string destination = output_command->GetParameterValue("dest");

			Unit* unit = getUnitFromID(object_to_move);
			Unit* dest = getUnitFromID(destination);

			if (unit != NULL && dest != NULL)
				unit->rightClick(dest); //Execute move command in starcraft
		}
		else if (name == "build") //Build command
		{
			string type = output_command->GetParameterValue("type");

			stringstream ss(type);
			int type_id;

			ss >> type_id;

			UnitType unit_type(type_id);

			if (output_command->FindByAttribute("location-x", 0))
			{
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
				worker->stop();

				if (!worker->build(unit_type, TilePosition(x,y)))
				{
					Error e = Broodwar->getLastError();
					cerr << "Error (BWAPI) (Build object): " << e.toString() << endl;
				}
			}
			else
			{
				string location = output_command->GetParameterValue("location");

				Unit* unit_location = getUnitFromID(location);

				if (!unit_location->train(unit_type))
				{
					Error e = Broodwar->getLastError();
					cerr << "Error (BWAPI) (Build object): " << e.toString() << endl;
				}
			}
		}

		output_command->AddStatusComplete();
	}

	//Update the units and resources
	SDL_mutexP(mu);
	time_end = clock();
	cout << "Time (4): " << (float(time_end) - float(time_start))/CLOCKS_PER_SEC << endl;
	update_units();
	time_end = clock();
	cout << "Time (5): " << (float(time_end) - float(time_start))/CLOCKS_PER_SEC << endl;
	update_resources();
	time_end = clock();
	cout << "Time (6): " << (float(time_end) - float(time_start))/CLOCKS_PER_SEC << endl;

	event_queue.update(); //Then have the events in the queue execute

	SDL_mutexV(mu);

	//test_input_file << "--------------------------------------------------" << endl;
}

void Soar_Link::print_soar(smlPrintEventId id, void *d, Agent *a, char const *m) //Print handler, handles all output of the agent
{
	string output(m);
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
