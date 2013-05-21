#include "stdafx.h"

#include "Choose_Filter.h"

#include <vector>

using namespace std;

//first = eliminate-list
//second = valid-list
pair<vector<building>, vector<building> > Choose_Filter::choose_within_distance_of(vector<building> grid, building within, float distance)
{
	vector<building> eliminate_list;
	vector<building> valid_list;

	float building_left_edge = within.building_position.first - distance;
	float building_right_edge = within.building_position.first + within.building_size.first + distance;

	float building_top_edge = within.building_position.second - distance;
	float building_bottom_edge = within.building_position.second + within.building_size.second + distance;

	for (vector<building>::iterator it = grid.begin();it != grid.end();)
	{
		int grid_left_edge = it->building_position.first;
		int grid_right_edge = it->building_position.first + it->building_size.first;

		int grid_top_edge = it->building_position.second;
		int grid_bottom_edge = it->building_position.second + it->building_size.second;

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