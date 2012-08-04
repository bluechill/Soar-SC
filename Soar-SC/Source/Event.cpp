#include "Event.h"

Soar_Event::Soar_Event(std::string cmd, bool svs_command)
	: command(cmd)
{
	if (svs_command)
		type = SVS_Command;
	else
		type = Console_Input;
}

Soar_Event::Soar_Event(sml::WMElement* element)
{
	this->element = element;
	type = WME_Destroy;
}

Soar_Event::Soar_Event(BWAPI::Unit* unit)
{
	this->bw_unit = unit;
	type = New_Unit;
}

std::string* Soar_Event::get_command()
{
	if (type == WME_Destroy)
		return NULL;
	else
		return &command;
}

sml::WMElement* Soar_Event::get_element()
{
	if (type != WME_Destroy)
		return NULL;
	else
		return element;
}

BWAPI::Unit* Soar_Event::get_unit()
{
	if (type != New_Unit)
		return NULL;
	else
		return bw_unit;
}

const Soar_Event::Event_Type Soar_Event::get_type()
{
	return type;
}