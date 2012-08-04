#ifndef EVENT_H
#define EVENT_H 1

#include <string>

#include "sml_Client.h"

#include "BWAPI.h"

class Soar_Event
{
public:
	Soar_Event(std::string command, bool svs_command = true);
	Soar_Event(sml::WMElement* element);
	Soar_Event(BWAPI::Unit* bw_unit);
	
	std::string* get_command();
	sml::WMElement* get_element();
	BWAPI::Unit* get_unit();

	//types
	typedef enum {
		SVS_Command = 0,
		WME_Destroy = 1,
		Console_Input = 2,
		New_Unit = 3
	} Event_Type;

	const Event_Type get_type();

private:
	std::string command;
	sml::WMElement* element;
	BWAPI::Unit* bw_unit;

	Event_Type type;
};

#endif