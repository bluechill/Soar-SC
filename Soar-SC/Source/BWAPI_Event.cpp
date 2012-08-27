#include "BWAPI_Event.h"

#include "Events.h"

#include <iostream>

BWAPI_Event::BWAPI_Event(BWAPI::UnitCommand command, sml::Identifier* id)
{
	internal_command = command;
	internal_id = id;
}

void BWAPI_Event::execute_command(Events* events)
{
	using namespace BWAPI;
	using namespace std;

	Unitset set;
	set.insert(internal_command.getUnit());
	if (!Broodwar->issueCommand(set, internal_command))
	{
		Error e = Broodwar->getLastError();
		cerr << "Got error trying to execute unit command: " << e.toString() << endl;

		if (internal_id != NULL)
			events->add_event(Soar_Event(internal_id, false));
	}
	else
	{
		if (internal_id != NULL)
			events->add_event(Soar_Event(internal_id, true));
	}
}

BWAPI::Unit* BWAPI_Event::get_unit()
{
	return internal_command.getUnit();
}