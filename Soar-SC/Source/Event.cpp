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

const Soar_Event::Event_Type Soar_Event::get_type()
{
	return type;
}