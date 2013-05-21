#include "stdafx.h"

#include "Multi_Choose_Filter.h"

#include <algorithm>

using namespace std;

std::pair<std::vector<building>, std::vector<building> > Multi_Choose_Filter::multi_choose_within_distance_of(std::vector<building> grid, vector<building> within, float distance)
{
	vector<building> eliminate_list;
	vector<building> valid_list;

	for (vector<building>::iterator it = within.begin();it != within.end();it++)
	{
		vector<building> one_valid_list = this->choose_within_distance_of(grid, *it, distance).second;

		valid_list.insert(valid_list.end(), one_valid_list.begin(), one_valid_list.end());
	}

	sort(valid_list.begin(), valid_list.end(), &Multi_Choose_Filter::buildings_are_greater);
	valid_list.erase(unique(valid_list.begin(), valid_list.end(), &Multi_Choose_Filter::buildings_are_the_same), valid_list.end());

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

bool Multi_Choose_Filter::buildings_are_the_same(building i, building j)
{
	return (i.building_position == j.building_position &&
		i.building_size == j.building_size);
}

bool Multi_Choose_Filter::buildings_are_greater(building i, building j)
{
	return (i.building_position < j.building_position);
}