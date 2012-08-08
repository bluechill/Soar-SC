#ifndef SOAR_UNIT_H
#define SOAR_UNIT_H 1

#include "BWAPI.h"

#include "sml_Client.h"

#include "Events.h"

class Soar_Unit
{
public:
	Soar_Unit(sml::Agent* agent, BWAPI::Unit* unit);
	~Soar_Unit();

	void delete_unit(Events *event_queue, sml::Agent* agent);

	void update(sml::Agent* agent, BWAPI::Unit* unit);

	const int get_id();

private:
	int id;

	bool building;

	bool idle;
	bool carrying;
	bool constructing;

	bool can_produce;
	bool full_queue;

	typedef struct {
		float x;
		float y;
	} Position;

	typedef Position Size;

	Position pos;
	Size size;
	BWAPI::UnitType type;

	std::string svsobject_id;

	sml::Identifier* get_unit_identifier(sml::Agent* agent, bool create_unit = false);

	bool deleted;
};

#endif
