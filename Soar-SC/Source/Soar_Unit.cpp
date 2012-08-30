#include "Soar_Unit.h"

#include "Soar_Link.h"
#include "Event.h"

#include <sstream>
#include <string>

#include <ctime>

using namespace BWAPI;
using namespace std;

Soar_Unit::Soar_Unit(sml::Agent* agent, Unit* unit, Soar_Link* link)
{
	this->link = link;

	using namespace sml;

	deleted = false;

	this->unit = unit;

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
		Unit* unit_building = unit->getBuildUnit();
		if (unit_building != NULL)
		{
			constructing = unit_building->getType().getID();
		}
	}

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
	size.y = float(type.dimensionUp()) + float(type.dimensionDown()) + 1.0f;
	size.x = float(type.dimensionLeft()) + float(type.dimensionRight()) + 1.0f;

	//Set the position
	pos.x = ((float)unit->getLeft()/32.0f);
	pos.y = Soar_Link::flip_one_d_point(((float)unit->getTop() + size.y)/32.0f, false);

	//Set the svsobject id
	svsobject_id = type.getName();
	svsobject_id.erase(remove_if(svsobject_id.begin(), svsobject_id.end(), isspace), svsobject_id.end());

	ss << id;
	svsobject_id += ss.str();

	//
	//Create the unit in SVS and on the input link
	//

	//Create the units
	Identifier* unit_id = get_unit_identifier(agent, true);

	unit_id->CreateIntWME("id", id);

	unit_id->CreateIntWME("idle", idle);
	unit_id->CreateIntWME("carrying", carrying);
	unit_id->CreateIntWME("constructing", constructing);

	unit_id->CreateIntWME("can-produce", can_produce);
	unit_id->CreateIntWME("full-queue", full_queue);

	ss.str("");
	ss << size.x/32.0f << " " << size.y/32.0f << " 1";
	string size = ss.str();
	ss.str("");

	ss << pos.x << " " << pos.y << " 0";
	string position = ss.str();

	string svs_command = "a " + svsobject_id + " world v " + Soar_Link::unit_box_verts + " p " + position + " s " + size + " r 0 0 0" + "\n";
	agent->SendSVSInput(svs_command);

	link->output_to_test_file(svs_command);
	
	unit_id->CreateIntWME("type", type.getID());

	unit_id->CreateStringWME("svsobject", svsobject_id.c_str());

	build = NULL;
}

Soar_Unit::~Soar_Unit()
{}

void Soar_Unit::update(sml::Agent* agent)
{
	using namespace sml;

	Identifier* unit_id = NULL;

	bool idle = unit->isIdle();
	if (this->idle != idle)
	{
		this->idle = idle;

		if (unit_id == NULL)
			unit_id = get_unit_identifier(agent);

		if (unit_id == NULL)
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

		if (unit_id == NULL)
			unit_id = get_unit_identifier(agent);

		if (unit_id == NULL)
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
		Unit* unit_building = unit->getBuildUnit();
		if (unit_building != NULL)
		{
			constructing = unit_building->getType().getID();
		}
	}
	
	if (this->constructing != constructing)
	{
		this->constructing = constructing;

		if (unit_id == NULL)
			unit_id = get_unit_identifier(agent);

		if (unit_id == NULL)
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

		if (elem == NULL && build->type.getID() == constructing)
		{
			build->build_id->AddStatusComplete();

			delete build;
			build = NULL;

			cerr << "NULLing Build from Complete!" << endl;
		}
		else if (elem == NULL && unit->isIdle())
		{
			build->build_id->AddStatusError();

			delete build;
			build = NULL;

			cerr << "NULLing Build from ERROR!" << endl;
		}
		else
			cerr << "Build exists.  Doing nothing though..., Orders: " << unit->getOrder().getName() << endl;
	}

	if (can_produce)
	{
		UnitType::set queue = unit->getTrainingQueue();

		size_t queue_size = queue.size();
		bool full_queue = (queue_size >= 5);
		if (this->full_queue != full_queue)
		{
			this->full_queue = full_queue;

			if (unit_id == NULL)
				unit_id = get_unit_identifier(agent);

			if (unit_id == NULL)
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
	pos.y = Soar_Link::flip_one_d_point(((float)unit->getTop() + size.y)/32.0f, false);
	if (this->pos.x != pos.x || this->pos.y != pos.y)
	{
		this->pos = pos;

		if (unit_id == NULL)
			unit_id = get_unit_identifier(agent);

		if (unit_id == NULL)
		{
			cerr << "Unable to get unit identifier: '" << id << "'.  So I was unable to update the unit.  Exiting the update function." <<endl;
			return;
		}


		stringstream ss;
		ss << pos.x << " " << pos.y << " 0";
		string position = ss.str();

		string svs_command = "c " + svsobject_id + " p " + position + "\n";
		link->output_to_test_file(svs_command);

		agent->SendSVSInput(svs_command);
	}
}

const int Soar_Unit::get_id()
{
	return id;
}

sml::Identifier* Soar_Unit::get_unit_identifier(sml::Agent* agent, bool create_unit)
{
	using namespace sml;

	Identifier* input_link = agent->GetInputLink();

	Identifier* units;
	if (!input_link->FindByAttribute("units", 0))
	{
		//Broodwar->printf("WARNING: No 'units' identifier on the input link! Creating....");
		cout << "WARNING: No 'units' identifier on the input link! Creating...." << endl;

		units = input_link->CreateIdWME("units");
	}
	else
		units = input_link->FindByAttribute("units", 0)->ConvertToIdentifier();

	Identifier* unit = NULL;

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
		return NULL;
}

void Soar_Unit::delete_unit(Events *event_queue, sml::Agent* agent)
{
	sml::Identifier* unit = get_unit_identifier(agent);

	string svs_command = "d " + svsobject_id + "\n";
	link->output_to_test_file(svs_command);

	event_queue->add_event(Soar_Event(svs_command, true));
	event_queue->add_event(Soar_Event(unit));
}

void Soar_Unit::will_build(build_struct* build)
{
	if (this->build != NULL || build == NULL)
		return;

	this->build = build;

	cerr << "Set build." << endl;
}