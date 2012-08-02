#include "Soar_Link.h"

#include <set>
#include <vector>
#include <sstream>

#include <windows.h>

using namespace BWAPI;
using namespace std;
using namespace sml;

int Soar_Link::soar_agent_thread()
{
	update_units();
	update_resources();

	while(!analyzer->done_sending_svs())
		Sleep(10);

	test_input_file << "--------------------------------------------------" << endl;

	agent->RunSelfForever();

	return 0;
}

void Soar_Link::output_handler(smlRunEventId id, void* d, Agent *a, smlPhase phase)
{
	int commands = a->GetNumberCommands();

	for (int i = 0;i < commands;i++)
	{
		Identifier* output_command = a->GetCommand(i);

        string name  = output_command->GetCommandName();

		if (name == "move")
		{
			string object_to_move = output_command->GetParameterValue("object");
			string destination = output_command->GetParameterValue("dest");

			Unit* unit = getUnitFromID(object_to_move);
			Unit* dest = getUnitFromID(destination);

			if (unit != NULL && dest != NULL)
				unit->rightClick(dest);
		}
	}

	SDL_mutexP(mu);
	update_units();
	update_resources();

	event_queue.update();
	
	SDL_mutexV(mu);

	test_input_file << "--------------------------------------------------" << endl;
}

void Soar_Link::print_soar(smlPrintEventId id, void *d, Agent *a, char const *m)
{
	string output(m);
	console->recieve_input(output);
}

void Soar_Link::misc_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase)
{
	switch (id)
	{
		case smlEVENT_AFTER_INTERRUPT:
		{
			event_queue.update_forever();
			had_interrupt = true;
			break;
		}
		case smlEVENT_BEFORE_RUNNING:
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
