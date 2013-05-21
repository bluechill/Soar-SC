//C++ Standard Library Headers
#include <sstream>
#include <stack>

//Soar SC Headers
#include "Soar_SC.h"
#include "Terrain.h"

//Windows Headers
#include <Windows.h>

using namespace std;
using namespace sml;

Terrain::Terrain(const std::vector<std::vector<bool> > &map, sml::Agent* agent) //Main constructor
	: out("bwapi-data/logs/test_input-terrain.txt") //Output file for the map
{
	if (!out.is_open()) //Check to make sure it is open otherwise stop working on the map
		return;

	this->map = map;

	this->agent = agent;

	this->should_die = false;
	this->done_svs = false;
}

Terrain::~Terrain() //Deconstructor
{
	if (!out.is_open())
		return;
}

void Terrain::analyze() //Main analyzer function, just creates a thread of the analyzer
{
	if (!out.is_open())
		return;

	mapping_function();
}

bool Terrain::rectangle_contains(const int x,const int y,vector<SVS_Rectangle> &rectangles) //Check whether a rectangle contains the given point
{
	SVS_Rectangle rect = get_rectangle(x,y,rectangles);
	
	if (rect.x == -1)
		return false;

	return true;
}

Terrain::SVS_Rectangle Terrain::get_rectangle(const int x, const int y,vector<SVS_Rectangle> &rectangles) //Get the rectangle containing the given point
{
	for (vector<SVS_Rectangle>::iterator it = rectangles.begin();it != rectangles.end();it++)
	{
		if (it->x <= x && x < it->x + it->size_x &&
			it->y <= y && y < it->y + it->size_y)
			return *it;
	}
	
	SVS_Rectangle rect;
	rect.x = -1;
	rect.y = -1;
	rect.size_x = -1;
	rect.size_y = -1;
	
	return rect;
}

void Terrain::generate_rectangle(const int x_start,const int y_start,vector<vector<bool> > &map, vector<SVS_Rectangle> &rectangles) //Generate the biggest rectangle from the given point
{
	vector<size_t> xs;
	vector<size_t> ys;

	for (size_t y = y_start;y < map.size();y++)
	{
		if (map[y][x_start] || rectangle_contains(x_start, y, rectangles))
			break;

		for (size_t x = x_start;x < map[y].size();x++)
		{
			if (map[y][x] || rectangle_contains(x, y, rectangles))
			{
				xs.push_back(x-x_start);
				break;
			}
		}
	}
	
	size_t min_x = map[0].size()-x_start;
	for (size_t i = 0;i < xs.size();i++)
	{
		if (xs[i] < min_x)
			min_x = xs[i];
	}
	
	for (size_t x = x_start;x < min_x + x_start && x < map.size();x++)
	{
		if (map[y_start][x] || rectangle_contains(x, y_start, rectangles))
			break;
		
		for (size_t y = y_start;y < map.size();y++)
		{
			if (map[y][x] || rectangle_contains(x, y, rectangles))
			{
				ys.push_back(y-y_start);
				break;
			}
		}
	}

	size_t min_y = map.size()-y_start;
	for (size_t i = 0;i < ys.size();i++)
	{
		if (ys[i] < min_y)
			min_y = ys[i];
	}
	
	SVS_Rectangle rect;
	rect.x = x_start;
	rect.y = y_start;
	rect.size_x = min_x ? min_x : 1;
	rect.size_y = min_y ? min_y : 1;

	rectangles.push_back(rect);
}

void Terrain::mapping_function() //Main map function generates the rectangles of the map
{
	vector<SVS_Rectangle> rectangles;

	vector<bool>* start_y = &map[0];

	size_t map_size = map.size();

	for (size_t y = 0;y < map_size && !should_die;y++,start_y++)
	{
		for (size_t x = 0;x < (*start_y).size() && !should_die;x++)
		{
			if (start_y->at(x) || rectangle_contains(x, y, rectangles))
				continue;

			generate_rectangle(x,y,map,rectangles);
		}
	}

	stringstream ss;

	Identifier* input_link = agent->GetInputLink();

	Identifier* terrain_store;

	if (!input_link->FindByAttribute("units", 0))
	{
		//Broodwar->printf("WARNING: No 'units' identifier on the input link! Creating....");
		cout << "WARNING: No 'units' identifier on the input link! Creating...." << endl;

		terrain_store = input_link->CreateIdWME("terrain");
	}
	else
		terrain_store = input_link->FindByAttribute("terrain", 0)->ConvertToIdentifier();

	int i = 0;
	for (vector<SVS_Rectangle>::iterator it = rectangles.begin();it != rectangles.end() && !should_die;i++, it++)
	{
		ss << i;
		string id = ss.str();
		ss.str("");
		string terrain_name = "TerrainRect" + id;

		SVS_Rectangle rect = *it;
		
		ss << ((float)rect.x + 2.0f)/4.0f << " " << Terrain::flip_one_d_point(((float)rect.y + (float)rect.size_y)/4.0f, false);

		string pos = ss.str();
		ss.str("");
		
		ss << (float)rect.size_x/4.0f << " " << (float)rect.size_y/4.0f;
		string size = ss.str();
		ss.str("");

		string svs_command = "a " + terrain_name + " terrain_object world v " + Terrain::unit_box_verts + " p " + pos + " 0 s " + size + " 1";
		agent->SendSVSInput(svs_command);

		terrain_store->CreateStringWME("object", terrain_name.c_str());
	}

	cout << "Rectangles: " << rectangles.size() << endl;

	done_svs = true;
}


bool Terrain::named_map_contains_point(named_point_map &map, pair<int,int> &point)
{
	map_point_array* internal_map = &(map.second);
	int internal_map_size = internal_map->size();

	for (int i = 0;i < internal_map_size;i++)
	{
		if (point.first == internal_map->at(i).first &&
			point.second == internal_map->at(i).second)
			return true;
	}

	return false;
}

bool Terrain::vector_named_map_contains_point(vector<named_point_map> &vector_map, pair<int,int> &point)
{
	int vector_map_size = vector_map.size();

	for (int i = 0;i < vector_map_size;i++)
	{
		if (named_map_contains_point(vector_map[i], point))
			return true;
	}

	return false;
}

void Terrain::flood_fill(std::pair<int,int> &location, std::vector<std::vector<bool> > &bool_map, bool target, named_point_map &fill_vector)
{
	stack<pair<int,int> > locations;

	if (location.second > int(bool_map.size()) || location.first > int(bool_map[location.second].size()) || location.first < 0 || location.second < 0)
		return;

	if (bool_map[location.second][location.first] != target)
		return;

	if (named_map_contains_point(fill_vector, location))
		return;

	locations.push(location);

	while (!locations.empty())
	{
		pair<int,int> n = locations.top();
		locations.pop();

		if (n.second > int(bool_map.size())				||
			n.first > int(bool_map[n.second].size())	||
			n.first < 0									||
			n.second < 0								||
			bool_map[n.second][n.first] != target		||
			named_map_contains_point(fill_vector, n)	)
			continue;

		pair<int,int> west,east;
		west = n;
		east = n;
		west.first--;
		east.first++;

		if (west.first >= 0)
		{
			while (	west.first > 0 &&
					bool_map[west.second][west.first] == target &&
					!named_map_contains_point(fill_vector, west))
			{
				fill_vector.second.push_back(west);
				west.first--;
			}
		}
		else
			west.first = 0;

		if (east.first < int(bool_map[n.second].size()))
		{
			while (	east.first < int(bool_map[n.second].size()) &&
					bool_map[east.second][east.first] == target &&
					!named_map_contains_point(fill_vector, east))
			{
				fill_vector.second.push_back(east);
				east.first++;
			}
		}
		else
			east.first = bool_map[n.second].size()-1;

		fill_vector.second.push_back(n);

		for (int x = west.first;x < east.first;x++)
		{
			if ((n.second+1) < int(bool_map.size()) &&
				bool_map[n.second+1][x] == target)
				locations.push(make_pair<int,int>(x,n.second+1));
			
			if ((n.second-1) >= 0 &&
				bool_map[n.second-1][x] == target)
				locations.push(make_pair<int,int>(x,n.second-1));
		}
	}

	//Done
}


const std::string Terrain::unit_box_verts = "0 0 0 0 0 1 0 1 0 0 1 1 1 0 0 1 0 1 1 1 0 1 1 1"; //The vertices of a 1x1x1 unit box

float Terrain::flip_one_d_point(const float &point, const bool &x_axis) //Flip a point around the map's half point in the x or y axis
{
	int map;

	if (x_axis)
		map = BWAPI::Broodwar->mapWidth();
	else
		map = BWAPI::Broodwar->mapHeight();

	float flipped_point = point;
	//Convert the point to be bewtween 0 and 1
	flipped_point /= map;
	//Subtract 0.5 so 0.5 in the point becomes 0
	flipped_point -= 0.5f;
	//Multiply by -1 to flip it around the axis
	flipped_point *= -1;
	//Add 0.5 again so it's from 0 to 1 again
	flipped_point += 0.5f;
	//Multiple by the map size in the axis to convert back to the point
	flipped_point *= map;

	//Return it
	return flipped_point;
}