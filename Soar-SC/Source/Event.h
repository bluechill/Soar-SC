#ifndef EVENT_H //Define EVENT_H to make sure we don't have duplicate includes
#define EVENT_H 1 //Set it to 1 (true) that we have EVENT_H

#include <string> //Include string for string related functions include std::string

#include "sml_Client.h" //For sml (Soar) related client stuff

#include "BWAPI.h" //For main interface to Starcraft

class Soar_Event //Class for handling different types of events
{
public:
	Soar_Event(std::string command, bool svs_command = true); //Construct an event around console commands or agent svs input
	Soar_Event(sml::WMElement* element); //Construct an event around destroying a working memory element
	Soar_Event(BWAPI::Unit* bw_unit); //Construct an event around adding a unit
	
	std::string* get_command(); //Get the console or SVS command
	sml::WMElement* get_element(); //Get the WME to be destroyed
	BWAPI::Unit* get_unit(); //Get the unit to add

	//All the event types
	typedef enum {
		SVS_Command = 0, //Agent SVS input
		WME_Destroy = 1, //Event to destroy a WME
		Console_Input = 2, //Console command
		New_Unit = 3 //Adding a new unit
	} Event_Type;

	const Event_Type get_type(); //Get the type

private:
	std::string command; //Internal string to hold the console or svs command
	sml::WMElement* element; //Internal WME pointer variable to hold the WME to destroy
	BWAPI::Unit* bw_unit; //Internal pointer to the unit to add

	Event_Type type; //Internal event type object
};

#endif