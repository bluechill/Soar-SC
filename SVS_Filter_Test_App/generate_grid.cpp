#include "stdafx.h"

#ifdef SVS
#include <iostream>
#include <map>
#include "filter.h"
#include "filter_table.h"
#include "common.h"
#include "scene.h"

using namespace std;

class generate_grid : public typed_map_filter<std::pair<int,int> > {
	typedef struct {
		int x;
		int y;
	} pos;
	typedef pos size;

	typedef struct {
		pos position;
		size size;
		string type;
		int id;
	} building;

	typedef struct {
		float x;
		float y;
	} Vector2D;

	typedef	struct {
		Vector2D center; // (X,Y) 
		Vector2D size; // (width, height)
		float angle; //Angle rotated by
	} Rotated_Rectangle;

	int round_int( double r ) {
		return int((r > 0.0) ? (r + 0.5) : (r - 0.5)); 
	}

public:
	generate_grid(Symbol *root, soar_interface *si, filter_input *input, scene *scn)
		: typed_map_filter<std::pair<int,int> >(root, si, input), scn(scn), seeded(false) {}

	bool seeded;

	bool compute(const filter_params *p, bool adding, std::pair<int,int> &res, bool &changed) {
		size shape_size;
		string type;

		if (!seeded)
		{
			seeded = true;
			srand(time(NULL));
		}

		if (!get_filter_param(this, p, "x-size", shape_size.x))
			return false;

		if (!get_filter_param(this, p, "y-size", shape_size.y))
			return false;

		if (!get_filter_param(this, p, "building-type", type))
			return false;

		//Middleware puts everything into SVS in the following ways
		//Buildings: Building<Is Enemy><Type><BWAPI ID>
		//<Is Enemy> is "Friend" when it's a friend and "Enemy" when it's an Enemy
		//<Type> is something like Terran_Marine
		//<BWAPI ID> is just a number like 5 or 99

		//Resources: Resource<Resource><BWAPI_ID>
		//<Resource> is either "Vesp" or "Minerals"
		//<BWAPI ID> is just a number like 5 or 99

		//Units: Unit<Is Enemy><Type><BWAPI_ID>
		//<Is Enemy> is "Friend" when it's a friend and "Enemy" when it's an Enemy
		//<Type> is something like Terran_Marine
		//<BWAPI ID> is just a number like 5 or 99

		//Fog of War Tile: BaseFogTile:<Middleware ID>
		//<Middleware ID> is just a number like 5:5 or 99:99 or 5:99

		//Terrain: TerrainRect<Middleware ID>
		//<Middleware ID> is just a number like 5 or 99

		//Terrain Borders: TerrainBorder<1-4>
		//<1-4> is the side the border is on
		//1 is the very bottom or "south" border
		//2 is the right or "east" border
		//3 is the top border or "north" border
		//4 is the left or "west" border

		vector<sgnode*> nodes;

		scn->get_all_nodes(nodes);

		vector<building> buildings;
		vector<building> resources;
		vector<building> terrain;

		for (vector<sgnode*>::iterator it = nodes.begin();it != nodes.end();it++)
		{
			string name = (*it)->get_name();

			if (strncmp(name.c_str(), "Building<Is Enemy><Type><BWAPI ID>", 8) == 0) // Only check for the "Building" part
			{
				if (strncmp(name.c_str()+8, "Friend", 6) != 0)
					continue; //It's an enemy so don't do anything

				//It is a building
				//Find the position (top left), size, type, and id
				ptlist points;
				(*it)->get_bounds().get_points(points);

				bool set_x = false;
				float neg_x; // Most negative x value of the bounding box
				float pos_x; // Most positive x value ... etc.

				bool set_y = false;
				float pos_y;
				float neg_y;

				for (int i = 0;i < points.size();i++)
				{
					if (set_x)
					{
						if (points[i](0) < neg_x)
							neg_x = points[i](0);
						else if (points[i](0) > pos_x)
							pos_x = points[i](0);
					}
					else
					{
						set_x = true;
						neg_x = pos_x = points[i](0);
					}

					if (set_y)
					{
						if (points[i](1) < neg_y)
							neg_y = points[i](1);
						else if (points[i](1) > pos_y)
							pos_y = points[i](1);
					}
					else
					{
						set_y = true;
						neg_y = pos_y = points[i](1);
					}
				}

				building object;

				object.position.x = round_int(neg_x);
				object.position.y = round_int(neg_y);

				object.size.x = round_int(pos_x) - round_int(neg_x);
				object.size.y = round_int(pos_y) - round_int(neg_y);

				//Get the type
				int i;
				for (i = 8/*Building*/+6/*Friend*/;!isdigit(name[i]);i++);

				object.type = name.substr(8+6, i-(8+6));

				string s_id = name.substr(i, name.length()-i); //Substring from i to the end of the string
				stringstream ss(s_id);

				int id;
				ss >> id;

				object.id = id;

				buildings.push_back(object);
			}
			else if (strncmp(name.c_str(), "Resource", 8) == 0)
			{
				//It's a resource
				ptlist points;
				(*it)->get_bounds().get_points(points);

				bool set_x = false;
				float neg_x; // Most negative x value of the bounding box
				float pos_x; // Most positive x value ... etc.

				bool set_y = false;
				float pos_y;
				float neg_y;

				for (int i = 0;i < points.size();i++)
				{
					if (set_x)
					{
						if (points[i](0) < neg_x)
							neg_x = points[i](0);
						else if (points[i](0) > pos_x)
							pos_x = points[i](0);
					}
					else
					{
						set_x = true;
						neg_x = pos_x = points[i](0);
					}

					if (set_y)
					{
						if (points[i](1) < neg_y)
							neg_y = points[i](1);
						else if (points[i](1) > pos_y)
							pos_y = points[i](1);
					}
					else
					{
						set_y = true;
						neg_y = pos_y = points[i](1);
					}
				}

				building object;

				object.position.x = round_int(neg_x);
				object.position.y = round_int(neg_y);

				object.size.x = round_int(pos_x) - round_int(neg_x);
				object.size.y = round_int(pos_y) - round_int(neg_y);

				resources.push_back(object);
			}
			else if (strncmp(name.c_str(), "Terrain", 7) == 0)
			{
				//It's part of the terrain
				//It's a resource
				ptlist points;
				(*it)->get_bounds().get_points(points);

				bool set_x = false;
				float neg_x; // Most negative x value of the bounding box
				float pos_x; // Most positive x value ... etc.

				bool set_y = false;
				float pos_y;
				float neg_y;

				for (int i = 0;i < points.size();i++)
				{
					if (set_x)
					{
						if (points[i](0) < neg_x)
							neg_x = points[i](0);
						else if (points[i](0) > pos_x)
							pos_x = points[i](0);
					}
					else
					{
						set_x = true;
						neg_x = pos_x = points[i](0);
					}

					if (set_y)
					{
						if (points[i](1) < neg_y)
							neg_y = points[i](1);
						else if (points[i](1) > pos_y)
							pos_y = points[i](1);
					}
					else
					{
						set_y = true;
						neg_y = pos_y = points[i](1);
					}
				}

				building object;

				object.position.x = round_int(neg_x);
				object.position.y = round_int(neg_y);

				object.size.x = round_int(pos_x) - round_int(neg_x);
				object.size.y = round_int(pos_y) - round_int(neg_y);

				terrain.push_back(object);
			}
		}

		//Generate the grid

		vector<building> grid = generate_complete_grid(buildings, shape_size);

		print_grid_to_file("initial.txt", grid, buildings, resources, terrain);

		//Remove any grids which conflict with the terrain or resources
		for (size_t i = 0;i < resources.size();i++)
		{
			int resource_left_edge = resources[i].position.x;
			int resource_right_edge = resources[i].position.x + resources[i].size.x - 1;

			int resource_top_edge = resources[i].position.y;
			int resource_bottom_edge = resources[i].position.y + resources[i].size.y - 1;

			for (vector<building>::iterator it = grid.begin();it != grid.end();)
			{
				int grid_left_edge = it->position.x;
				int grid_right_edge = it->position.x + it->size.x;

				int grid_top_edge = it->position.y;
				int grid_bottom_edge = it->position.y + it->size.y;

				if (grid_right_edge < resource_left_edge ||
					grid_left_edge > resource_right_edge ||
					grid_bottom_edge < resource_top_edge ||
					grid_top_edge > resource_bottom_edge ) //No collision exists, no overlap
				{
					it++;
				}
				else
					it = grid.erase(it);
			}
		}

		print_grid_to_file("resource_conflict.txt", grid, buildings, resources, terrain, false);

		for (size_t i = 0;i < terrain.size();i++)
		{
			int terrain_left_edge = terrain[i].position.x;
			int terrain_right_edge = terrain[i].position.x + terrain[i].size.x - 1;

			int terrain_top_edge = terrain[i].position.y;
			int terrain_bottom_edge = terrain[i].position.y + terrain[i].size.y - 1;

			for (vector<building>::iterator it = grid.begin();it != grid.end();)
			{
				int grid_left_edge = it->position.x;
				int grid_right_edge = it->position.x + it->size.x;

				int grid_top_edge = it->position.y;
				int grid_bottom_edge = it->position.y + it->size.y;

				if (grid_right_edge < terrain_left_edge ||
					grid_left_edge > terrain_right_edge ||
					grid_bottom_edge < terrain_top_edge ||
					grid_top_edge > terrain_bottom_edge ) //No collision exists, no overlap
				{
					it++;
				}
				else
					it = grid.erase(it);
			}
		}

		print_grid_to_file("terrain_conflict.txt", grid, buildings, resources, terrain, false);

		//Do a fuzzy line intersect to make sure nothing conflicts with the paths to the resources, but if they do, remove them

		if (resources.size() > 0)
		{
			vector<building> bases;

			for (int i = 0;i < buildings.size();i++)
			{
				if (buildings[i].type == "Terran_Command_Center")
					bases.push_back(buildings[i]);
			}

			for (int i = 0;i < bases.size();i++)
			{
				building cc = bases[i];

				pair<vector<building>, vector<building> > resources_within_5_of_cc = choose_within_distance_of(resources, cc, 5);

				pair<vector<building>, vector<building> > list_pair = multi_fuzzy_line_intersect(cc, resources_within_5_of_cc.second, 1, grid);

				vector<building> to_eliminate = list_pair.first;

				for (vector<building>::iterator it = to_eliminate.begin();it != to_eliminate.end();it++)
				{
					for (vector<building>::iterator jt = grid.begin();jt != grid.end();)
					{
						if (buildings_are_the_same(*it, *jt))
							jt = grid.erase(jt);
						else
							jt++;
					}
				}
			}
		}

		//Debug info
		//Output the complete map including the grids

		print_grid_to_file("resource_path_conflict.txt", grid, buildings, resources, terrain);

		//Check whether buildings of the type we're going to build exist, if they do, only mark valid grids near (within 2 squares) of those buildings valid
		vector<building> buildings_of_same_type;

		for (int i = 0;i < buildings.size();i++)
		{
			if (buildings[i].type == type)
				buildings_of_same_type.push_back(buildings[i]);
		}

		if (buildings_of_same_type.size() > 0)
		{
			pair<vector<building>, vector<building> > list_pair = multi_choose_within_distance_of(grid, buildings_of_same_type, 1+(shape_size.x > shape_size.y ? shape_size.x : shape_size.y));

			vector<building> to_eliminate = list_pair.first;

			for (vector<building>::iterator it = to_eliminate.begin();it != to_eliminate.end();it++)
			{
				for (vector<building>::iterator jt = grid.begin();jt != grid.end();)
				{
					if (buildings_are_the_same(*it, *jt))
						jt = grid.erase(jt);
					else
						jt++;
				}
			}
		}

		//Randomly choose one out of all of the grids
		assert(grid.size() > 0);

		//Debug info
		//Output the complete map including the grids

		print_grid_to_file("final.txt", grid, buildings, resources, terrain);

		assert(0);

		unsigned int random_grid = rand()%grid.size();
		building final_grid = grid[random_grid];

		changed = (res.first != final_grid.position.x ||
			res.second != final_grid.position.y);
		res = make_pair<int,int>(final_grid.position.x, final_grid.position.y);

		return true;
	}

private:
	void print_grid_to_file(string file_name, vector<building> grid, vector<building> buildings, vector<building> resources, vector<building> terrain, bool print_grid_first = true)
	{
		vector<vector<char> > map;

		for (size_t y = 0;y <= 64;y++)
		{
			vector<char> y_row;

			for (size_t x = 0;x <= 64;x++)
				y_row.push_back(' ');

			map.push_back(y_row);
		}

		if (print_grid_first)
		{
			for (size_t i = 0;i < grid.size();i++)
			{
				for (size_t y = grid[i].position.y;y < (grid[i].position.y + grid[i].size.y);y++)
				{
					for (size_t x = grid[i].position.x;x < (grid[i].position.x + grid[i].size.x);x++)
						map[y][x] = 'G';
				}
			}
		}

		for (size_t i = 0;i < buildings.size();i++)
		{
			for (size_t y = buildings[i].position.y;y < (buildings[i].position.y + buildings[i].size.y);y++)
			{
				for (size_t x = buildings[i].position.x;x < (buildings[i].position.x + buildings[i].size.x);x++)
					map[y][x] = 'B';
			}
		}

		for (size_t i = 0;i < resources.size();i++)
		{
			for (size_t y = resources[i].position.y;y < (resources[i].position.y + resources[i].size.y);y++)
			{
				for (size_t x = resources[i].position.x;x < (resources[i].position.x + resources[i].size.x);x++)
					map[y][x] = 'R';
			}
		}

		for (size_t i = 0;i < terrain.size();i++)
		{
			for (size_t y = terrain[i].position.y;y < (terrain[i].position.y + terrain[i].size.y);y++)
			{
				for (size_t x = terrain[i].position.x;x < (terrain[i].position.x + terrain[i].size.x);x++)
					map[y][x] = 'T';
			}
		}

		if (!print_grid_first)
		{
			for (size_t i = 0;i < grid.size();i++)
			{
				for (size_t y = grid[i].position.y;y < (grid[i].position.y + grid[i].size.y);y++)
				{
					for (size_t x = grid[i].position.x;x < (grid[i].position.x + grid[i].size.x);x++)
						map[y][x] = 'G';
				}
			}
		}

		ofstream file("bwapi-data/logs/" + file_name);

		for (size_t y = 0;y < 64;y++)
		{
			for (size_t x = 0;x < 64;x++)
				file << map[y][x];

			file << endl;
		}
	}

	vector<building> generate_complete_grid(vector<building> building_locations, size shape_size)
	{
		vector<building> final_grid;

		if (building_locations.size() == 0 || shape_size.x == 0 || shape_size.y == 0)
			return final_grid;

		for (size_t i = 0;i < building_locations.size();i++)
		{
			vector<building> grid = generate_individual_grid(building_locations[i].position, shape_size, 1, building_locations[i].size);

			final_grid.insert(final_grid.end(), grid.begin(), grid.end());
		}

		//Remove duplicates
		//sort(final_grid.begin(), final_grid.end(), &generate_grid::buildings_are_greater);
		//final_grid.erase(unique(final_grid.begin(), final_grid.end(), &generate_grid::buildings_are_the_same), final_grid.end());

		//Remove building locations as free
		for (size_t i = 0;i < building_locations.size();i++)
		{
			int building_left_edge = building_locations[i].position.x;
			int building_right_edge = building_locations[i].position.x + building_locations[i].size.x;

			int building_top_edge = building_locations[i].position.y;
			int building_bottom_edge = building_locations[i].position.y + building_locations[i].size.y;

			for (vector<building>::iterator it = final_grid.begin();it != final_grid.end();)
			{
				int grid_left_edge = it->position.x;
				int grid_right_edge = it->position.x + it->size.x;

				int grid_top_edge = it->position.y;
				int grid_bottom_edge = it->position.y + it->size.y;

				if (grid_right_edge < building_left_edge ||
					grid_left_edge > building_right_edge ||
					grid_bottom_edge < building_top_edge ||
					grid_top_edge > building_bottom_edge ) //No collision exists, no overlap
				{
					it++;
				}
				else
					it = final_grid.erase(it);
			}
		}

		return final_grid;
	}

	vector<building> generate_individual_grid(pos location, size grid_size, int tile_buffer, size shape)
	{
		vector<building> grid;

		if (grid_size.x == 0 || grid_size.y == 0 || shape.x == 0 || shape.y == 0)
			return grid;

		for (int y = (location.y - tile_buffer - grid_size.y);y < (location.y + shape.y - 1 + tile_buffer + grid_size.y);y++)
		{
			for (int x = (location.x - tile_buffer - grid_size.x);x < (location.x + shape.x - 1 + tile_buffer + grid_size.x - 1);x++)
			{
				building temp_building;

				temp_building.position.x = x;
				temp_building.position.y = y;
				temp_building.size = grid_size;

				grid.push_back(temp_building);
			}
		}

		return grid;
	}

	std::pair<std::vector<building>, std::vector<building> > multi_fuzzy_line_intersect(building object1, std::vector<building> generation_objects, float fuzziness, std::vector<building> grids)
	{
		vector<building> eliminate_list;
		vector<building> valid_list;

		if (generation_objects.size() == 0 || grids.size() == 0)
			return make_pair<vector<building>, vector<building> >(eliminate_list, valid_list);

		for (vector<building>::iterator it = generation_objects.begin();it != generation_objects.end();it++)
		{
			pair<vector<building>, vector<building> > lists = this->fuzzy_line_intersect(object1, *it, fuzziness, grids);

			eliminate_list.insert(eliminate_list.end(), lists.first.begin(), lists.first.end());
		}

		//sort(eliminate_list.begin(), eliminate_list.end(), &generate_grid::buildings_are_greater);
		//eliminate_list.erase(unique(eliminate_list.begin(), eliminate_list.end(), &generate_grid::buildings_are_the_same), eliminate_list.end());

		for (vector<building>::iterator it = grids.begin();it != grids.end();it++)
		{
			bool found = false;

			for (vector<building>::iterator jt = eliminate_list.begin();jt != eliminate_list.end();jt++)
			{
				if (buildings_are_the_same(*it, *jt))
				{
					found = true;
					break;
				}
			}

			if (!found)
				valid_list.push_back(*it);
		}

		pair<vector<building>, vector<building> > result;
		result.first = eliminate_list;
		result.second = valid_list;

		return result;
	}

	std::pair<std::vector<building>, std::vector<building> > fuzzy_line_intersect(building object1, building object2, float fuzziness, std::vector<building> grids)
	{
		vector<building> eliminate_list;
		vector<building> valid_list;

		if (grids.size() == 0)
			return make_pair<vector<building>, vector<building> >(eliminate_list, valid_list);

		//Generate the rotated rectangle for doing collision tests
		//Rotated_Rectangle line_rect;

		//Calculate centers of two objects
		float object1_center_x = float(object1.position.x) + float(object1.size.x)/2.0f;
		float object1_center_y = float(object1.position.y) + float(object1.size.y)/2.0f;

		float object2_center_x = float(object2.position.x) + float(object2.size.x)/2.0f;
		float object2_center_y = float(object2.position.y) + float(object2.size.y)/2.0f;

		////Average value between the two buildings is the center
		//line_rect.center.x = (object1_center_x + object2_center_y)/2.0f;
		//line_rect.center.y = (object1_center_y + object2_center_y)/2.0f;

		////Absolute value of the difference between the two buildings is the size
		//line_rect.size.x = abs(object2_center_x - object1_center_x);
		//line_rect.size.y = abs(object2_center_y - object1_center_y);

		//1 = Top Right
		//2 = Top Left
		//3 = Bottom Left
		//4 = Bottom Right
		int quadrant = 1;

		if (object1.position.y > object2.position.y)
		{
			//Bottom Quadrants
			if (object1.position.x > object2.position.x)
				quadrant = 3; //Bottom Left
			else
				quadrant = 4; //Bottom Right
		}
		else
		{
			//Top Quadrants
			if (object1.position.x > object2.position.x)
				quadrant = 2; // Top Left Quadrant
			else
				quadrant = 1; // Top Right Quadrant
		}

		//float angle_addition = 0.0f;

		//const float pi = atan(1.0f)*4.0f; // tan inverse of 1 is pi/4

		////sin = opposite / hypo
		////opposite = y in this case
		//float y = line_rect.size.y;

		////Calculate the abs hypo
		//float x = line_rect.size.x;

		//float hypo = sqrt(pow(x, 2) + pow(y, 2));

		////Nothing to do for Quadrant 1
		//if (quadrant == 2)
		//	angle_addition = pi/2;
		//else if (quadrant == 3)
		//	angle_addition = pi;
		//else if (quadrant == 4)
		//	angle_addition = 3/2 * pi;

		//float angle = asinf(y/hypo) + angle_addition;

		//line_rect.angle = angle;

		//Compensate for the fuzziness
		//Calculate new line size

		//Calculate the top point first

		//Generate vector between two points
		Vector2D V;
		V.x = object2_center_x - object1_center_x;
		V.y = object2_center_y - object1_center_y;

		//Generate the perpendicular
		Vector2D P;
		P.x = V.y;
		P.y = V.x;

		//Normalize the vector
		double length = sqrt(P.x*P.x + P.y*P.y);
		Vector2D N;
		N.x = abs(P.x/length);
		N.y = abs(P.y/length);

		if (quadrant == 1)
			N.x = -N.x;
		else if (quadrant == 3)
			N.y = -N.y;
		else if (quadrant == 4)
		{
			N.x = -N.x;
			N.y = -N.y;
		}

		//Because I know how this is going to work, I can make this just two line intersects.
		//If you *really* want 2D rectangle collisions you have to fix the commented out code!

		Vector2D top;
		top.x = object2_center_x + N.x * fuzziness / 2.0f;
		top.y = object2_center_y + N.y * fuzziness / 2.0f;

		Vector2D bottom;
		bottom.x = object1_center_x + N.x * fuzziness / 2.0f;
		bottom.y = object1_center_y + N.y * fuzziness / 2.0f;

		Vector2D top_2;
		top_2.x = object2_center_x - N.x * fuzziness / 2.0f;
		top_2.y = object2_center_y - N.y * fuzziness / 2.0f;

		Vector2D bottom_2;
		bottom_2.x = object1_center_x - N.x * fuzziness / 2.0f;
		bottom_2.y = object1_center_y - N.y * fuzziness / 2.0f;

		////Calculate the vector to the top point from the second point

		////Calculate the vector from the center to the top point
		//Vector2D size;
		//size.x = top.x - line_rect.center.x;
		//size.y = top.y - line_rect.center.y;

		////Set the size
		//line_rect.size = size;

		//Calculate all intersects
		for (vector<building>::iterator it = grids.begin();it != grids.end();it++)
		{
			//Rotated_Rectangle grid;
			//grid.angle = 0.0f;
			//grid.center.x = float(it->position.x) + float(it->size.x)/2.0f;
			//grid.center.y = float(it->position.y) + float(it->size.y)/2.0f;

			//grid.size.x = float(it->size.x)/2.0f;
			//grid.size.y = float(it->size.y)/2.0f;

			float grid_min_x = it->position.x;
			float grid_max_x = it->position.x + it->size.x;
			
			float grid_min_y = it->position.y;
			float grid_max_y = it->position.y + it->size.y;

			//if (Rotated_Rectangle_Collision(&line_rect, &grid))
			if (SegmentIntersectRectangle(grid_min_x, grid_min_y, grid_max_x, grid_max_y, bottom.x, bottom.y, top.x, top.y) ||
				SegmentIntersectRectangle(grid_min_x, grid_min_y, grid_max_x, grid_max_y, bottom_2.x, bottom_2.y, top_2.x, top_2.y))
			{
				eliminate_list.push_back(*it);
			}
			else
				valid_list.push_back(*it);
		}

		pair<vector<building>, vector<building> > result;
		result.first = eliminate_list;
		result.second = valid_list;

		return result;
	}

	bool SegmentIntersectRectangle(	double rect_min_x		,
									double rect_min_y		,
									double rect_max_x		,
									double rect_max_y		,
									double point1_x				,
									double point1_y				,
									double point2_x				,
									double point2_y				)
	{
		// Find min and max X for the segment

		double minX = point1_x;
		double maxX = point2_x;

		if(point1_x > point2_x)
		{
			minX = point2_x;
			maxX = point1_x;
		}

		// Find the intersection of the segment's and rectangle's x-projections

		if(maxX > rect_max_x)
		{
			maxX = rect_max_x;
		}

		if(minX < rect_min_x)
		{
			minX = rect_min_x;
		}

		if(minX > maxX) // If their projections do not intersect return false
		{
			return false;
		}

		// Find corresponding min and max Y for min and max X we found before

		double minY = point1_y;
		double maxY = point1_y;

		double dx = point2_x - point1_x;

		if(abs(dx) > 0.0000001)
		{
			double a = (point2_y - point1_y) / dx;
			double b = point1_y - a * point1_x;
			minY = a * minX + b;
			maxY = a * maxX + b;
		}

		if(minY > maxY)
		{
			double tmp = maxY;
			maxY = minY;
			minY = tmp;
		}

		// Find the intersection of the segment's and rectangle's y-projections

		if(maxY > rect_max_y)
		{
			maxY = rect_max_y;
		}

		if(minY < rect_min_y)
		{
			minY = rect_min_y;
		}

		if(minY > maxY) // If Y-projections do not intersect return false
		{
			return false;
		}

		return true;
	}

	std::pair<std::vector<building>, std::vector<building> > multi_choose_within_distance_of(std::vector<building> grid, std::vector<building> within, float distance)
	{
		vector<building> eliminate_list;
		vector<building> valid_list;

		if (grid.size() == 0 || within.size() == 0)
			return make_pair<vector<building>, vector<building> >(eliminate_list, valid_list);

		for (vector<building>::iterator it = within.begin();it != within.end();it++)
		{
			vector<building> one_valid_list = this->choose_within_distance_of(grid, *it, distance).second;

			valid_list.insert(valid_list.end(), one_valid_list.begin(), one_valid_list.end());
		}

		//sort(valid_list.begin(), valid_list.end(), &generate_grid::buildings_are_greater);
		//valid_list.erase(unique(valid_list.begin(), valid_list.end(), &generate_grid::buildings_are_the_same), valid_list.end());

		for (vector<building>::iterator it = grid.begin();it != grid.end();it++)
		{
			bool found = false;

			for (vector<building>::iterator jt = valid_list.begin();jt != valid_list.end();jt++)
			{
				if (buildings_are_the_same(*it, *jt))
				{
					found = true;
					break;
				}
			}

			if (!found)
				eliminate_list.push_back(*it);
		}

		pair<vector<building>, vector<building> > result;
		result.first = eliminate_list;
		result.second = valid_list;

		return result;
	}

	std::pair<std::vector<building>, std::vector<building> > choose_within_distance_of(std::vector<building> grid, building within, float distance)
	{
		vector<building> eliminate_list;
		vector<building> valid_list;

		if (grid.size() == 0)
			return make_pair<vector<building>, vector<building> >(eliminate_list, valid_list);

		float building_left_edge = within.position.x - distance;
		float building_right_edge = within.position.x + within.size.x + distance;

		float building_top_edge = within.position.y - distance;
		float building_bottom_edge = within.position.y + within.size.y + distance;

		for (vector<building>::iterator it = grid.begin();it != grid.end();it++)
		{
			int grid_left_edge = it->position.x;
			int grid_right_edge = it->position.x + it->size.x;

			int grid_top_edge = it->position.y;
			int grid_bottom_edge = it->position.y + it->size.y;

			if (grid_right_edge < building_left_edge ||
				grid_left_edge > building_right_edge ||
				grid_bottom_edge < building_top_edge ||
				grid_top_edge > building_bottom_edge ) //no overlap, not within
			{
				eliminate_list.push_back(*it);
			}
			else
				valid_list.push_back(*it);
		}

		pair<vector<building>, vector<building> > result;
		result.first = eliminate_list;
		result.second = valid_list;

		return result;
	}

	static bool buildings_are_the_same(building i, building j)
	{
		return (i.position.x == j.position.x &&
				i.position.y == i.position.y &&
				i.size.x == j.size.x &&
				i.size.y == j.size.y);
	}

	static bool buildings_are_greater(building i, building j)
	{
		return (i.position.x < j.position.x);
	}

	//Rotated Rectangle Code
	//Method from http://www.ragestorm.net/tutorial?id=22

	inline void Add_Vectors2D(Vector2D *v1, Vector2D *v2)
	{
		v1->x += v2->x;
		v1->y += v2->y;
	}

	inline void Subtract_Vectors2D(Vector2D *v1, Vector2D *v2)
	{
		v1->x -= v2->x;
		v1->y -= v2->y;
	}

	inline void Rotate_Vector2D_Clockwise(Vector2D *v, float angle)
	{
		float vector_x = v->x;
		float cos_angle = cos(angle);
		float sin_angle = sin(angle);

		v->x = vector_x * cos_angle + v->y * sin_angle;
		v->y = -vector_x * sin_angle + v->y * cos_angle;
	}

	bool Rotated_Rectangle_Collision(Rotated_Rectangle *rect1, Rotated_Rectangle *rect2)
	{
		//Copy the given variables so we don't touch them
		Rotated_Rectangle rectangle_1 = *rect1;
		Rotated_Rectangle rectangle_2 = *rect2;

		//Translate the plane by the negative of rectangle 1's center
		//Now rectangle 1 is at the origin
		Subtract_Vectors2D(&rectangle_2.center, &rectangle_1.center);
		Subtract_Vectors2D(&rectangle_1.center, &rectangle_1.center);

		//Rotate the plane by the negative of rectangle 2's angle
		//Now rectangle 2 is axis-aligned and rectangle 1 is at the origin
		Rotate_Vector2D_Clockwise(&rectangle_1.center, -rectangle_2.angle);
		Rotate_Vector2D_Clockwise(&rectangle_2.center, -rectangle_2.angle);

		rectangle_1.angle -= rectangle_2.angle;
		rectangle_2.angle -= rectangle_2.angle;

		//Calculate the vertices of	the now rotated rectangle 2
		Vector2D bottom_left;
		Vector2D top_right;

		bottom_left = rectangle_2.center;
		top_right = rectangle_2.center;

		Subtract_Vectors2D(&bottom_left, &rectangle_2.size);
		Add_Vectors2D(&top_right, &rectangle_2.size);

		//Since Rectangle 1 is centered on the origin, we can define two pairs
		//of vertices, A and B, such that A is the vertical minimum and -A is
		//the vertical max and B is the horizontal minimum and -B is the
		//horizontal maximum

		Vector2D A;
		Vector2D B;

		float sin_angle = sin(rectangle_1.angle);
		float cos_angle = cos(rectangle_1.angle);

		A.x = -rectangle_1.size.y * sin_angle;
		B.x = A.x;

		float t = rectangle_1.size.x * cos_angle;
		A.x += t;
		B.x -= t;

		A.y = rectangle_1.size.y * cos_angle;
		B.y = A.y;

		t = rectangle_1.size.x * sin_angle;
		A.y += t;
		B.y -= t;

		t = sin_angle * cos_angle;

		//Make sure A is the vert min/max and B is the horiz min/max
		if (t < 0)
		{
			t = A.x;
			A.x = B.x;
			B.x = t;

			t = A.y;
			A.y = B.y;
			B.y = t;
		}

		//Make sure B is the horiz min
		if (sin_angle < 0)
		{
			B.x = -B.x;
			B.y = -B.y;
		}

		//If rectangle 2 isn't between the left and right of rectangle 1, collision is impossible
		if (B.x >= top_right.x ||
			B.x <= bottom_left.x)
		{
			return false;
		}

		float rect1_vert_min;
		float rect1_vert_max;

		//If rectangle 1 is axis-aligned as well, then we can easily find it's vert min and max
		if (t == 0) //t is sin_angle * cos_angle
		{
			rect1_vert_min = A.y;
			rect1_vert_max = -A.y;
		}
		else
		{
			float x = bottom_left.x - A.x;
			float a = top_right.x - A.x;

			rect1_vert_min = A.y;
			//If the vertical min/max isn't on the range of the rectangle (cross rectangle check),
			//then find it on that range (has to be at bottom_left.x or top_right.x)
			if (a * x > 0)
			{
				float dx = A.x;
				if (x < 0)
				{
					dx -= B.x;
					rect1_vert_min -= B.y;
					x = a;
				}
				else
				{
					dx += B.x;
					rect1_vert_min += B.y;
				}

				rect1_vert_min *= x;
				rect1_vert_min /= dx;
				rect1_vert_min += A.y;
			}

			x = bottom_left.x + A.x;
			a = top_right.x + A.x;

			rect1_vert_max = -A.y;
			// if the second vertical min/max isn't in (BL.x, TR.x), then
			// find the local vertical min/max on BL.x or on TR.x
			if (a*x > 0)
			{
				float dx = -A.x;
				if (x < 0)
				{
					dx -= B.x;
					rect1_vert_max -= B.y;
					x = a;
				}
				else
				{
					dx += B.x;
					rect1_vert_max += B.y;
				}

				rect1_vert_max *= x;
				rect1_vert_max /= dx;
				rect1_vert_max -= A.y;
			}
		}

		//Return true if the vert max or vert min is within rect 2
		if (rect1_vert_min <= bottom_left.y &&
			rect1_vert_max <= bottom_left.y)
			return false;
		else if (rect1_vert_min >= top_right.y &&
			rect1_vert_max >= top_right.y)
			return false;

		return true;
	}

	scene *scn;
};

filter *make_generate_grid(Symbol *root, soar_interface *si, scene *scn, filter_input *input) {
	return new generate_grid(root, si, input, scn);
}

filter_table_entry *generate_grid_fill_entry() {
	filter_table_entry *e = new filter_table_entry;
	e->name = "generate_grid";
	e->parameters.push_back("x-size");
	e->parameters.push_back("y-size");
	e->parameters.push_back("building-type");
	e->ordered = false;
	e->allow_repeat = false;
	e->create = &make_generate_grid;
	e->calc = NULL;
	return e;
}


#endif
