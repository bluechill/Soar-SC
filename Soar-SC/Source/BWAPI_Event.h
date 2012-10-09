#ifndef BWAPI_EVENT_H
#define BWAPI_EVENT_H

#include "BWAPI.h"

#include "sml_Client.h"

class Events;
class Soar_Link;

class BWAPI_Event
{
public:
	BWAPI_Event(BWAPI::UnitCommand command, sml::Identifier* id, Soar_Link* link); //Construct a BWAPI_Event around a command for a unit

	BWAPI::Unit* get_unit();

	void execute_command(Events* events);

private:
	int id;

	BWAPI::Position pos;
	BWAPI::UnitCommand internal_command;
	sml::Identifier *internal_id;
	Soar_Link* internal_link;
};

#endif
