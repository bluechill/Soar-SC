#ifndef BWAPI_EVENT_H
#define BWAPI_EVENT_H

#include "BWAPI.h"
#include "sml_Client.h"

class Soar_SC;
class Soar_Link;

class BWAPI_Event
{
public:
	BWAPI_Event(BWAPI::UnitCommand command, sml::Identifier* id, Soar_SC* link); //Construct a BWAPI_Event around a command for a unit

	BWAPI::Unit get_unit();

	void execute_command();

	int get_mineral_usage();
	int get_gas_usage();

private:
	int id;

	BWAPI::Position pos;
	BWAPI::UnitCommand internal_command;
	sml::Identifier *internal_id;
	Soar_SC* soar_sc_link;
};

#endif
