#ifndef SOAR_UNIT_H
#define SOAR_UNIT_H 1

#include "BWAPI.h"

#include "sml_Client.h"

#include "Events.h"

class Soar_Link;

class Soar_Unit
{
public:
	Soar_Unit(sml::Agent* agent, BWAPI::Unit* unit, Soar_Link* link, bool enemy);
	~Soar_Unit();

	void delete_unit(Events *event_queue, sml::Agent* agent);

	void update(sml::Agent* agent);

	const int get_id();

	typedef struct {
		sml::Identifier* build_id;
		BWAPI::UnitType type;
	} build_struct;

	void will_build(build_struct* build);

	typedef struct {
		float x;
		float y;
	} Position;

	Position lastPosition() { return pos; }

private:
	Soar_Link* link;

	BWAPI::Unit* unit;

	int id;

	bool building;

	bool idle;
	bool carrying;
	int constructing;

	bool can_produce;
	bool full_queue;

	typedef Position Size;

	Position pos;
	Size size;
	BWAPI::UnitType type;

	std::string svsobject_id;

	sml::Identifier* get_unit_identifier(sml::Agent* agent, bool create_unit = false, bool enemy = false);

	bool deleted;

	build_struct* build;

	bool isEnemy;

	int targetID;
};

#endif
