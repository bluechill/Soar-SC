//C++ Standard Library Headers
#include <sstream>
#include <string>

//C Standard Library Headers
#include <ctime>

//BWAPI Headers
#include <BWAPI.h>

//Soar SC Headers
#include "Soar_Unit.h"
#include "Soar_SC.h"
#include "Soar_Link.h"
#include "Terrain.h"

using namespace BWAPI;
using namespace std;

Soar_Unit::Soar_Unit(Soar_SC* soar_sc_link, Unit unit, bool enemy)
{
	this->soar_sc_link = soar_sc_link;
	this->isEnemy = enemy;

	using namespace sml;

	deleted = false;

	this->unit = unit;
	size = new Size;
	pos = new Position;

	if (unit == nullptr)
		return; //This is just a container for something like a fog tile

	//Variable used a lot for conversion of ints to strings and strings to ints
	stringstream ss;

	//Set the id
	id = unit->getID();

	//Set the main booleans
	idle = unit->isIdle();
	carrying = (unit->isCarryingGas() || unit->isCarryingMinerals() || unit->getPowerUp());
	constructing = -1;

	if (unit->isConstructing())
	{
		Unit unit_building = unit->getBuildUnit();
		if (unit_building != nullptr)
		{
			constructing = unit_building->getType().getID();
		}
	}

	if (unit->getTarget() != nullptr)
		targetID = unit->getTarget()->getID();
	else
		targetID = 0;

	//Set the type
	type = unit->getType();

	cout << "Adding unit '" << type.getName() << id << "'" << endl;

	//Set whether it's a building or not
	building = type.isBuilding();

	//Set whether it can produce and then the queue size
	can_produce = type.canProduce();

	UnitType::set queue = unit->getTrainingQueue();
	size_t queue_size = queue.size();
	full_queue = (queue_size >= 5);

	//Set the size
	size->y = float(type.tileHeight());
	size->x = float(type.tileWidth());

	//Set the position
	pos->x = ((float)unit->getLeft()/32.0f);
	pos->y = Terrain::flip_one_d_point(((float)unit->getTop() + size->y - 1)/32.0f, false);

	if (building)
		svsobject_id = "Building";
	else
		svsobject_id = "Unit";

	//Set the svsobject id
	if (enemy)
		svsobject_id += "Enemy";
	else
		svsobject_id += "Friend";

	svsobject_id += type.getName();
	svsobject_id.erase(remove_if(svsobject_id.begin(), svsobject_id.end(), isspace), svsobject_id.end());

	ss << id;
	svsobject_id += ss.str();

	//
	//Create the unit in SVS and on the input link
	//

	//Create the units
	Identifier* unit_id = get_unit_identifier(true, enemy);

	unit_id->CreateIntWME("id", id);

	unit_id->CreateIntWME("idle", idle);
	unit_id->CreateIntWME("carrying", carrying);
	unit_id->CreateIntWME("constructing", constructing);

	unit_id->CreateIntWME("can-produce", can_produce);
	unit_id->CreateIntWME("full-queue", full_queue);
	unit_id->CreateIntWME("target", targetID);

	ss.str("");
	ss << size->x << " " << size->y << " 1";
	string size = ss.str();
	ss.str("");

	ss << pos->x << " " << pos->y << " 0";
	string position = ss.str();

	string svs_command = "a " + svsobject_id + " bwapi_unit world v " + Terrain::unit_box_verts + " p " + position + " s " + size + " r 0 0 0" + "\n";
	soar_sc_link->get_soar_link()->SendSVSInput(svs_command);

	unit_id->CreateIntWME("type", type.getID());

	unit_id->CreateStringWME("svsobject", svsobject_id.c_str());

	build = nullptr;
}

Soar_Unit::~Soar_Unit()
{
	delete pos;
	delete size;
}

void Soar_Unit::update()
{
	using namespace sml;

	if (unit == nullptr)
		return; //This is just a container for something like a fog tile

	Identifier* unit_id = nullptr;

	bool idle = unit->isIdle();
	if (this->idle != idle)
	{
		this->idle = idle;

		if (unit_id == nullptr)
			unit_id = get_unit_identifier(false, isEnemy);

		if (unit_id == nullptr)
		{
			cerr << "Unable to get unit identifier: '" << id << "'.  So I was unable to update the unit.  Exiting the update function." <<endl;
			return;
		}

		IntElement* idle_int = unit_id->FindByAttribute("idle", 0)->ConvertToIntElement();
		idle_int->Update(idle);
	}

	bool carrying = (unit->isCarryingGas() || unit->isCarryingMinerals() || unit->getPowerUp());
	if (this->carrying != carrying)
	{
		this->carrying = carrying;

		if (unit_id == nullptr)
			unit_id = get_unit_identifier(false, isEnemy);

		if (unit_id == nullptr)
		{
			cerr << "Unable to get unit identifier: '" << id << "'.  So I was unable to update the unit.  Exiting the update function." <<endl;
			return;
		}

		IntElement* carrying_int = unit_id->FindByAttribute("carrying", 0)->ConvertToIntElement();
		carrying_int->Update(carrying);
	}

	int constructing = -1;

	if (unit->isConstructing())
	{
		Unit unit_building = unit->getBuildUnit();
		if (unit_building != nullptr)
		{
			constructing = unit_building->getType().getID();
		}
	}
	
	if (this->constructing != constructing)
	{
		this->constructing = constructing;

		if (unit_id == nullptr)
			unit_id = get_unit_identifier(false, isEnemy);

		if (unit_id == nullptr)
		{
			cerr << "Unable to get unit identifier: '" << id << "'.  So I was unable to update the unit.  Exiting the update function." <<endl;
			return;
		}

		IntElement* constructing_int = unit_id->FindByAttribute("constructing", 0)->ConvertToIntElement();
		constructing_int->Update(constructing);
	}

	if (build)
	{
		WMElement* elem = build->build_id->FindByAttribute("status", 0);

		if (elem == nullptr && build->type.getID() == constructing)
		{
			build->build_id->AddStatusComplete();

			delete build;
			build = nullptr;

			cerr << "NULLing Build from Complete!" << endl;
		}
		else if (elem == nullptr && unit->isIdle())
		{
			build->build_id->AddStatusError();

			delete build;
			build = nullptr;

			cerr << "NULLing Build from ERROR!" << endl;
		}
		else if (elem != nullptr)
			cerr << "Build Status Exists already with value '" << elem->GetValueAsString() << "'.  Orders are: " << unit->getOrder().getName() << endl;
	}

	if (can_produce)
	{
		UnitType::set queue = unit->getTrainingQueue();

		size_t queue_size = queue.size();
		bool full_queue = (queue_size >= 5);
		if (this->full_queue != full_queue)
		{
			this->full_queue = full_queue;

			if (unit_id == nullptr)
				unit_id = get_unit_identifier(false, isEnemy);

			if (unit_id == nullptr)
			{
				cerr << "Unable to get unit identifier: '" << id << "'.  So I was unable to update the unit.  Exiting the update function." <<endl;
				return;
			}

			IntElement* full_queue_int = unit_id->FindByAttribute("full-queue", 0)->ConvertToIntElement();
			full_queue_int->Update(full_queue);
		}
	}

	Soar_Unit::Position pos;

	int size_y = type.dimensionUp() + type.dimensionDown() + 1;

	pos.x = ((float)unit->getLeft()/32.0f);
	pos.y = Terrain::flip_one_d_point(((float)unit->getTop() + size->y)/32.0f, false);
	if (abs(this->pos->x - pos.x) > 0.5f || abs(this->pos->y - pos.y) > 0.5f)
	{
		this->pos->x = pos.x;
		this->pos->y = pos.y;

		if (unit_id == nullptr)
			unit_id = get_unit_identifier(false, isEnemy);

		if (unit_id == nullptr)
		{
			cerr << "Unable to get unit identifier: '" << id << "'.  So I was unable to update the unit.  Exiting the update function." <<endl;
			return;
		}

		stringstream ss;
		ss << pos.x << " " << pos.y << " 0";
		string position = ss.str();

		string svs_command = "c " + svsobject_id + " p " + position + "\n";

		soar_sc_link->get_soar_link()->SendSVSInput(svs_command);
	}

	Unit target = unit->getTarget();
	int newTargetID = 0;
	if (target != nullptr)
		newTargetID = target->getID();
	else
	{
		target = unit->getOrderTarget();

		if (target != nullptr)
			newTargetID = target->getID();
	}

	if (newTargetID != targetID)
	{
		if (unit_id == nullptr)
			unit_id = get_unit_identifier(false, isEnemy);

		targetID = newTargetID;

		IntElement* wme = unit_id->FindByAttribute("target", 0)->ConvertToIntElement();
		wme->Update(targetID);
	}
}

const int Soar_Unit::get_id()
{
	if (unit == nullptr)
		return -1; //This is just a container for something like a fog tile

	return id;
}

sml::Identifier* Soar_Unit::get_unit_identifier(bool create_unit, bool enemy)
{
	using namespace sml;

	Identifier* input_link = soar_sc_link->get_soar_link()->GetInputLink();

	Identifier* units;
	if (!enemy)
	{
		if (!input_link->FindByAttribute("units", 0))
		{
			//Broodwar->printf("WARNING: No 'units' identifier on the input link! Creating....");
			cout << "WARNING: No 'units' identifier on the input link! Creating...." << endl;

			units = input_link->CreateIdWME("units");
		}
		else
			units = input_link->FindByAttribute("units", 0)->ConvertToIdentifier();
	}
	else
	{
		if (!input_link->FindByAttribute("enemy-units", 0))
		{
			//Broodwar->printf("WARNING: No 'units' identifier on the input link! Creating....");
			cout << "WARNING: No 'units' identifier on the input link! Creating...." << endl;

			units = input_link->CreateIdWME("enemy-units");
		}
		else
			units = input_link->FindByAttribute("enemy-units", 0)->ConvertToIdentifier();
	}

	Identifier* unit = nullptr;

	for (int i = 0;i < units->GetNumberChildren();i++)
	{
		unit = units->GetChild(i)->ConvertToIdentifier();

		WMElement* id_wme = unit->FindByAttribute("id", 0);
		if (!id_wme)
			continue;
			
		IntElement* id_int = id_wme->ConvertToIntElement();

		int unit_id = int(id_int->GetValue());

		if (unit_id == id)
			return unit;
	}

	if (create_unit)
	{
		if (building)
			unit = units->CreateIdWME("building");
		else
			unit = units->CreateIdWME("unit");

		return unit;
	}
	else
		return nullptr;
}

void Soar_Unit::delete_unit()
{
	if (unit == nullptr)
		return; //This is just a container for something like a fog tile

	sml::Identifier* unit = get_unit_identifier(false, isEnemy);

	string svs_command = "d " + svsobject_id + "\n";

	soar_sc_link->add_event(Soar_Event(svs_command, true));
	soar_sc_link->add_event(Soar_Event(unit));
}

void Soar_Unit::will_build(build_struct* build)
{
	if (unit == nullptr)
		return; //This is just a container for something like a fog tile

	if (this->build != nullptr || build == nullptr)
		return;

	this->build = build;

	cerr << "Set build." << endl;
}

void Soar_Unit::set_svsobject_id(std::string svsobject_id)
{
	if (this->svsobject_id != "")
		return; //Only allow this if the unit doesn't have an svs object id

	this->svsobject_id = svsobject_id;
}

void Soar_Unit::set_position(Position pos)
{
	if (svsobject_id != "")
		return; //Only allow this if the unit doesn't have an svs object id

	this->pos->x = pos.x;
	this->pos->y = pos.y;
}

void Soar_Unit::set_size(Size size)
{
	if (svsobject_id != "")
		return; //Only allow this if the unit doesn't have an svs object id

	this->size->x = size.x;
	this->size->y = size.y;
}
