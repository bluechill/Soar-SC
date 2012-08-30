#include "BWAPI_Event.h"

#include "Soar_Link.h"
#include "Soar_Unit.h"

#include "Events.h"

#include <iostream>

BWAPI_Event::BWAPI_Event(BWAPI::UnitCommand command, sml::Identifier* id, Soar_Link* link)
{
	internal_command = command;
	internal_id = id;
	internal_link = link;
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

	if (internal_command.getType() != UnitCommandTypes::Build && internal_id != NULL)
		events->add_event(Soar_Event(internal_id, true));
	else if (internal_command.getType() == UnitCommandTypes::Build && internal_id != NULL)
	{
		Soar_Unit::build_struct* build = new Soar_Unit::build_struct;
		build->type = internal_command.getUnitType();
		build->build_id = internal_id;

		internal_link->get_units()[internal_command.getUnit()]->will_build(build);
	}
}

BWAPI::Unit* BWAPI_Event::get_unit()
{
	return internal_command.getUnit();
}