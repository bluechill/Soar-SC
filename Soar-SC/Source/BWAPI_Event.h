#ifndef BWAPI_EVENT_H
#define BWAPI_EVENT_H

#include "BWAPI.h"

#include "sml_Client.h"

class Events;

class BWAPI_Event
{
public:
	BWAPI_Event(BWAPI::UnitCommand command, sml::Identifier* id); //Construct a BWAPI_Event around a command for a unit

	BWAPI::Unit* get_unit();

	void execute_command(Events* events);

private:
	BWAPI::UnitCommand internal_command;
	sml::Identifier *internal_id;
};

#endif
