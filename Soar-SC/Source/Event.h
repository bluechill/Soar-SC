#ifndef EVENT_H
#define EVENT_H 1

#include <string>

#include "sml_Client.h"

class Soar_Event
{
public:
	Soar_Event(std::string command, bool svs_command = true);
	Soar_Event(sml::WMElement* element);
	
	std::string* get_command();
	sml::WMElement* get_element();

	//types
	typedef enum {
		SVS_Command = 0,
		WME_Destroy = 1,
		Console_Input = 2
	} Event_Type;

	const Event_Type get_type();

private:
	std::string command;
	sml::WMElement* element;

	Event_Type type;
};

#endif