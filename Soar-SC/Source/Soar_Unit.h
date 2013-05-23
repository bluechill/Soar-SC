#ifndef SOAR_UNIT_H
#define SOAR_UNIT_H 1

//BWAPI Headers
#include <BWAPI.h>

//SML Headers
#include "sml_Client.h"

//Soar SC Headers & Stubs
class Soar_SC;

class Soar_Unit
{
public:
	Soar_Unit(Soar_SC* soar_sc_link, BWAPI::Unit* unit, bool enemy);
	~Soar_Unit();

	void delete_unit();

	void update();

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

	typedef Position Size;

	Position lastPosition() { return pos; }
	Size get_size() { return size; }

	std::string get_svsobject_id() { return svsobject_id; }

private:
	Soar_SC* soar_sc_link;

	BWAPI::Unit* unit;

	int id;

	bool building;

	bool idle;
	bool carrying;
	int constructing;

	bool can_produce;
	bool full_queue;

	Position pos;
	Size size;
	BWAPI::UnitType type;

	std::string svsobject_id;

	sml::Identifier* get_unit_identifier(bool create_unit = false, bool enemy = false);

	bool deleted;

	build_struct* build;

	bool isEnemy;

	int targetID;
};

#endif
