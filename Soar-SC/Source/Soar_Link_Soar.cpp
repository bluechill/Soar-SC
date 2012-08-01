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

	/*SDL_cond* output_condition = event_queue.get_output_condition();
	SDL_mutex* event_mu = event_queue.get_mutex();

	event_queue.update_forever();

	SDL_mutexP(event_mu);
	SDL_CondWait(output_condition, event_mu);
	SDL_mutexV(event_mu);

	event_queue.update_only_on_calls();
*/
	SDL_mutexV(mu);

	test_input_file << "--------------------------------------------------" << endl;
}

void Soar_Link::print_soar(smlPrintEventId id, void *d, Agent *a, char const *m)
{
	string output(m);

	size_t result = output.find('\n');
	if (result == string::npos)
		output += "\r\n";
	else
		output.insert(output.begin(), '\r');

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
