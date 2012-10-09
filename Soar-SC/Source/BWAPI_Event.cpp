#include "BWAPI_Event.h"

#include "Soar_Link.h"
#include "Soar_Unit.h"

#include "Events.h"

#include <iostream>

BWAPI_Event::BWAPI_Event(BWAPI::UnitCommand command, sml::Identifier* id, Soar_Link* link)
{
	internal_command = command;

	if (internal_command.getTarget() != NULL)
		this->id = internal_command.getTarget()->getID();
	else
		this->id = -1;

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
		
		if (e == Errors::Unit_Does_Not_Exist)
		{
			std::map<int, BWAPI::Position> copyHiddenMap = internal_link->get_last_positions();
			std::map<int, BWAPI::Position>::iterator hidden_it = copyHiddenMap.find(id);
			if (hidden_it != copyHiddenMap.end())
			{
				internal_command = UnitCommand::move(internal_command.getUnit(), hidden_it->second);

				this->execute_command(events);
				return;
			}
			else
			{
				cerr << "Got error trying to execute unit command: " << e.toString() << endl;

				if (internal_id != NULL)
					events->add_event(Soar_Event(internal_id, false));

				return;
			}
		}
		else
		{
			cerr << "Got error trying to execute unit command: " << e.toString() << endl;

			if (internal_id != NULL)
				events->add_event(Soar_Event(internal_id, false));

			return;
		}
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