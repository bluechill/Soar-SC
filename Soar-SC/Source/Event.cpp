#include "Event.h" //Include for the header

Soar_Event::Soar_Event(std::string cmd, bool svs_command) //Constructor of the event
	: command(cmd) //Initialize the internal string with the given string
{
	if (svs_command) //If this is an SVS command
		type = SVS_Command; //Set the type to be an SVS Command
	else //Otherwise
		type = Console_Input; //Set this to be a console command
}

Soar_Event::Soar_Event(sml::WMElement* element) //Constructor of the event
{
	this->element = element; //Set the internal variable pointer to the given one
	type = WME_Destroy; //Set our type to be WME_Destroy
}

Soar_Event::Soar_Event(BWAPI::Unit* unit) //Constructor of the event
{
	this->bw_unit = unit; //Set the internal variable pointer to the given one
	type = New_Unit; //Set our type to be New_Unit
}

std::string* Soar_Event::get_command() //Return a pointer to the string of the command
{
	if (type != SVS_Command && type != Console_Input) //If this event is not a SVS Command or Console Input
		return NULL; //Then return NULL because the command isn't a string command
	else //Otherwise
		return &command; //Return a pointer to the command
}

sml::WMElement* Soar_Event::get_element() //Return a pointer to the WMElement
{
	if (type != WME_Destroy) //If the type isn't a WME_Destroy type
		return NULL; //Return NULL because this isn't the correct type (not WME_Destroy)
	else //Otherwise
		return element; //Return our variable pointer
}

BWAPI::Unit* Soar_Event::get_unit() //Return a pointer to the unit to be added
{
	if (type != New_Unit) //If this isn't a new unit event type
		return NULL; //Return NULL
	else //Otherwise
		return bw_unit; //return the internal variable pointer
}

const Soar_Event::Event_Type Soar_Event::get_type() //Return the type of the event
{
	return type; //Return the internal variable of the type cast to be const
}
