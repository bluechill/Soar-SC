#include "stdafx.h"

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>

#include <ctime>

#include "Grid_Filter.h"

using namespace std;

Grid_Filter::Grid_Filter()
{}

Grid_Filter::~Grid_Filter()
{}

vector<building> Grid_Filter::generate_complete_grid(vector<building> building_locations)
{
	vector<building> final_grid;

	for (size_t i = 0;i < building_locations.size();i++)
	{
		vector<building> grid = generate_grid(building_locations[i].building_position, make_pair<int,int>(1,1), 2, building_locations[i].building_size);

		final_grid.insert(final_grid.end(), grid.begin(), grid.end());
	}

	//Remove duplicates
	sort(final_grid.begin(), final_grid.end(), &Grid_Filter::buildings_are_greater);
	final_grid.erase(unique(final_grid.begin(), final_grid.end(), &Grid_Filter::buildings_are_the_same), final_grid.end());

	//Remove building locations as free
	for (size_t i = 0;i < building_locations.size();i++)
	{
		int building_left_edge = building_locations[i].building_position.first;
		int building_right_edge = building_locations[i].building_position.first + building_locations[i].building_size.first;

		int building_top_edge = building_locations[i].building_position.second;
		int building_bottom_edge = building_locations[i].building_position.second + building_locations[i].building_size.second;

		for (vector<building>::iterator it = final_grid.begin();it != final_grid.end();)
		{
			int grid_left_edge = it->building_position.first;
			int grid_right_edge = it->building_position.first + it->building_size.first;

			int grid_top_edge = it->building_position.second;
			int grid_bottom_edge = it->building_position.second + it->building_size.second;

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

vector<building> Grid_Filter::generate_grid(pos location, size grid_size, int tile_buffer, size shape)
{
	vector<building> grid;

	for (int y = (location.second - tile_buffer - grid_size.second + 1);y < (location.second + shape.second - 1 + tile_buffer + grid_size.second - 1*(grid_size.second-1));y++)
	{
		for (int x = (location.first - tile_buffer - grid_size.first + 1);x < (location.first + shape.first - 1 + tile_buffer + grid_size.first - 1*(grid_size.first-1));x++)
		{
			building temp_building;

			temp_building.building_position = make_pair<int,int>(x,y);
			temp_building.building_size = grid_size;

			grid.push_back(temp_building);
		}
	}

	return grid;
}

bool Grid_Filter::buildings_are_the_same(building i, building j)
{
	return (i.building_position == j.building_position &&
		i.building_size == j.building_size);
}

bool Grid_Filter::buildings_are_greater(building i, building j)
{
	return (i.building_position < j.building_position);
}