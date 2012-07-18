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

	update_units();
	update_resources();
}