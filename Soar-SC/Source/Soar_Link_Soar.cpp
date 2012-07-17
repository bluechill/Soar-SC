#include "Soar_Link.h"

#include <set>
#include <vector>
#include <sstream>

using namespace BWAPI;
using namespace std;
using namespace sml;

void Soar_Link::add_resource(int bw_id, int count, BWAPI::Position position, BWAPI::UnitType type, float angle)
{
	Identifier* input_link = agent->GetInputLink();

	Identifier* id;
	if (!input_link->FindByAttribute("resources", 0))
	{
		Broodwar->printf("WARNING: No 'units' identifier on the input link! Creating....");
		cout << "WARNING: No 'units' identifier on the input link! Creating...." << endl;

		id = input_link->CreateIdWME("resources");
	}
	else
		id = input_link->FindByAttribute("resources", 0)->ConvertToIdentifier();

	string name;

	if (type.getName().find("Mineral") != string::npos)
		name = "mineral";
	else
		name = "vesp-gas";

	Identifier* resource = id->CreateIdWME(name.c_str());

	resource->CreateIntWME("id", bw_id);
	resource->CreateIntWME("count", count);

	string svs_object_id = type.getName();
	svs_object_id.erase(remove_if(svs_object_id.begin(), svs_object_id.end(), isspace), svs_object_id.end());

	stringstream ss;
	ss << bw_id;
	svs_object_id += ss.str();
	ss.str("");
	//Flip the point so "north" isn't negative y
	ss << ((float)position.x)/32.0f << " " << flip_one_d_point(((float)position.y)/32.0f, false) << " 0";
	string position_svs = ss.str();
	ss.str("");

	ss << ((float)(type.dimensionLeft() + type.dimensionRight() + 1))/32.0f << " " << ((float)(type.dimensionUp() + type.dimensionDown() + 1))/32.0f << " 1";
	string size = ss.str();
	ss.str("");


	ss << angle;
	string rotation = ss.str();

	string svs_command = "a " + svs_object_id + " world v " + unit_box_verts + " p " + position_svs + " s " + size + " r 0 " + rotation + " 0";
	//Broodwar->printf("%s", svs_command.c_str());
	cout << svs_command << endl;

	SDL_mutexP(mu);
	agent->SendSVSInput(svs_command);
	SDL_mutexV(mu);

	resource->CreateStringWME("svsobject", svs_object_id.c_str());

	test_input_file << "I-resources-" << name << ": ^svsobject " << svs_object_id << endl;

	test_input_file << "SVS-Actual: " << svs_command << endl;
}

void Soar_Link::delete_resource(int bw_id)
{
	Identifier* input_link = agent->GetInputLink();

	Identifier* id;

	if (!input_link->FindByAttribute("resources", 0))
	{
		cout << "ERROR: No 'resources' identifier on the input link! Creating...." << endl;

		id = input_link->CreateIdWME("resources");
	}
	else
		id = input_link->FindByAttribute("resources", 0)->ConvertToIdentifier();

	for (int j = 0;j < id->GetNumberChildren();j++)
	{
		Identifier* unit;
		if (!id->GetChild(j)->IsIdentifier())
			continue;
		else
			unit = id->GetChild(j)->ConvertToIdentifier();

		if (unit->FindByAttribute("id", 0)->ConvertToIntElement()->GetValue() == bw_id)
		{
			string svs_object_id = unit->FindByAttribute("svsobject", 0)->ConvertToStringElement()->GetValue();

			string svs_command = "d " + svs_object_id;

			test_input_file << "SVS-Actual: " << svs_command << endl;

			SDL_mutexP(mu);
			agent->SendSVSInput(svs_command);
			SDL_mutexV(mu);

			id->GetChild(j)->DestroyWME();

			break;
		}
	}
}

void Soar_Link::update_resources()
{
	Unitset visible_minerals = Broodwar->getMinerals();
	Unitset visible_vesp_gas = Broodwar->getGeysers();

	Unitset new_minerals;
	Unitset new_vesp_gas;

	for (Unitset::iterator it = visible_minerals.begin();it != visible_minerals.end();it++)
	{
		if (minerals.find(*it) == minerals.end())
		{
			//Doesn't exist in my current list of visible minerals

			Unit* bw_unit = (*it);

			add_resource(bw_unit->getID(), bw_unit->getResources(), Position(bw_unit->getLeft(), bw_unit->getTop()), bw_unit->getType(), (float)bw_unit->getAngle());
		}

		new_minerals.push_back(*it);
	}

	for (Unitset::iterator it = visible_vesp_gas.begin();it != visible_vesp_gas.end();it++)
	{
		if (vesp_gas.find(*it) == vesp_gas.end())
		{
			//Doesn't exist in my current list of visible vespian gas

			Unit* bw_unit = (*it);

			add_resource(bw_unit->getID(), bw_unit->getResources(), Position(bw_unit->getLeft(), bw_unit->getTop()), bw_unit->getType(), (float)bw_unit->getAngle());
		}

		new_vesp_gas.push_back(*it);
	}

	Unitset final_minerals;

	for (Unitset::iterator it = new_minerals.begin();it != new_minerals.end();it++)
	{
		if (!(*it)->exists())
			delete_resource((*it)->getID());
		else
			final_minerals.insert(*it);
	}

	Unitset final_vesp_gas;

	for (Unitset::iterator it = new_vesp_gas.begin();it != new_vesp_gas.end();it++)
	{
		if (!(*it)->exists())
			delete_resource((*it)->getID());
		else
			final_vesp_gas.insert(*it);
	}

	minerals = final_minerals;
	vesp_gas = final_vesp_gas;
}

void Soar_Link::update_units()
{
	Unitset my_units_new = Broodwar->self()->getUnits();

	for (Unitset::iterator it = my_units_new.begin();it != my_units_new.end();it++)
	{
		if (my_units.find(*it) == my_units.end())
		{
			Unit* bw_unit = (*it);

			Identifier* input_link = agent->GetInputLink();

			Identifier* id;
			if (!input_link->FindByAttribute("units", 0))
			{
				//Broodwar->printf("WARNING: No 'units' identifier on the input link! Creating....");
				cout << "WARNING: No 'units' identifier on the input link! Creating...." << endl;

				id = input_link->CreateIdWME("units");
			}
			else
				id = input_link->FindByAttribute("units", 0)->ConvertToIdentifier();

			Identifier* unit;

			if (!bw_unit->getType().isBuilding())
			{
				unit = id->CreateIdWME("unit");

				test_input_file << "I-units-unit:";

				unit->CreateIntWME("id", bw_unit->getID());
				test_input_file << " ^id " << bw_unit->getID();

				bool worker = bw_unit->getType().isWorker();
				unit->CreateIntWME("worker", worker);
				test_input_file << " ^worker " << worker;

				if (worker)
				{
					unit->CreateIntWME("idle", bw_unit->isIdle());
					test_input_file << " ^idle " << bw_unit->isIdle();

					unit->CreateIntWME("carrying", (bw_unit->isCarryingGas() || bw_unit->isCarryingMinerals() || bw_unit->getPowerUp()));
					test_input_file << " ^carring " << (bw_unit->isCarryingGas() || bw_unit->isCarryingMinerals() || bw_unit->getPowerUp());

					if (bw_unit->isCarryingGas())
					{
						unit->CreateStringWME("carrying", "gas");
						test_input_file << " ^carring gas";
					}
					else if (bw_unit->isCarryingMinerals())
					{
						unit->CreateStringWME("carring", "minerals");
						test_input_file << " ^carrying minerals";
					}
					else if (bw_unit->getPowerUp())
					{
						unit->CreateStringWME("carrying", "powerup");
						test_input_file << " ^carrying powerup";
					}
				}
				else
				{
					//TODO handle units other than workers
				}
			}
			else
			{
				unit = id->CreateIdWME("building");

				test_input_file << "I-units-building:";

				unit->CreateIntWME("id", bw_unit->getID());
				test_input_file << " ^id " << bw_unit->getID();
			}

			string svs_object_id = bw_unit->getType().getName();
			svs_object_id.erase(remove_if(svs_object_id.begin(), svs_object_id.end(), isspace), svs_object_id.end());

			stringstream ss;
			ss << bw_unit->getID();
			svs_object_id += ss.str();
			ss.str("");
			//Flip the point so "north" isn't negative y
			ss << ((float)bw_unit->getLeft()/32.0f) << " " << flip_one_d_point(((float)bw_unit->getBottom())/32.0f, false) << " 0";
			string position = ss.str();
			ss.str("");

			ss << ((float)(bw_unit->getType().dimensionLeft() + bw_unit->getType().dimensionRight() + 1))/32.0f << " " << ((float)(bw_unit->getType().dimensionUp() + bw_unit->getType().dimensionDown() + 1))/32.0f << " 1";
			string size = ss.str();
			ss.str("");

			ss << bw_unit->getAngle();
			string rotation = ss.str();

			string svs_command = "a " + svs_object_id + " world v " + unit_box_verts + " p " + position + " s " + size + " r 0 " + rotation + " 0";
			//Broodwar->printf("%s", svs_command.c_str());
			cout << svs_command << endl;

			SDL_mutexP(mu);
			agent->SendSVSInput(svs_command);
			SDL_mutexV(mu);

			unit->CreateStringWME("svsobject", svs_object_id.c_str());

			test_input_file << " ^svsobject " << svs_object_id << endl;

			test_input_file << "SVS-Actual: " << svs_command << endl;
		}
		else
		{
			Unit* orig = *my_units.find(*it);
			Unit* new_unit = *it;

			string svs_object_id = new_unit->getType().getName();
			svs_object_id.erase(remove_if(svs_object_id.begin(), svs_object_id.end(), isspace), svs_object_id.end());

			stringstream ss;
			ss << new_unit->getID();
			svs_object_id += ss.str();
			ss.str("");

			string svs_command = "c " + svs_object_id;

			if (orig->getLeft() != new_unit->getLeft() ||
				orig->getTop() != new_unit->getTop())
			{
				ss << ((float)new_unit->getLeft()/32.0f) << " " << flip_one_d_point(((float)new_unit->getTop())/32.0f, false) << " 0";
				string position = ss.str();
				ss.str("");

				svs_command += " " + position;
			}

			if (orig->getAngle() != new_unit->getAngle())
			{
				ss << new_unit->getAngle();
				string rotation = ss.str();

				svs_command += " 0 " + rotation + " 0";
			}

			if (svs_command != ("c " + svs_object_id))
			{
				SDL_mutexP(mu);
				agent->SendSVSInput(svs_command);
				SDL_mutexV(mu);
				test_input_file << "SVSAction: " << svs_command << endl;
			}
		}

		my_units_new.insert(*it);
	}

	Unitset final_units;

	for (Unitset::iterator it = my_units_new.begin(), it_next = it;it != my_units_new.end();it++)
	{
		Identifier* input_link = agent->GetInputLink();

		if (!(*it)->exists())
		{
			Identifier* id;

			if (!input_link->FindByAttribute("units", 0))
			{
				cout << "ERROR: No 'units' identifier on the input link! Creating...." << endl;

				id = input_link->CreateIdWME("units");
			}
			else
				id = input_link->FindByAttribute("units", 0)->ConvertToIdentifier();

			for (int j = 0;j < id->GetNumberChildren();j++)
			{
				Identifier* unit;
				if (!id->GetChild(j)->IsIdentifier())
					continue;
				else
					unit = id->GetChild(j)->ConvertToIdentifier();

				if (unit->FindByAttribute("id", 0)->ConvertToIntElement()->GetValue() == (*it)->getID())
				{
					string svs_object_id = unit->FindByAttribute("svsobject", 0)->ConvertToStringElement()->GetValue();

					string svs_command = "d " + svs_object_id;

					test_input_file << "SVS-Actual: " << svs_command << endl;

					SDL_mutexP(mu);
					agent->SendSVSInput(svs_command);
					SDL_mutexV(mu);

					id->GetChild(j)->DestroyWME();

					break;
				}
			}
		}
		else
			final_units.insert(*it);
	}

	my_units = final_units;
}

int Soar_Link::soar_agent_thread()
{
	update_units();
	update_resources();

	agent->RunSelfForever();

	return 0;
}

void Soar_Link::output_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase)
{

}