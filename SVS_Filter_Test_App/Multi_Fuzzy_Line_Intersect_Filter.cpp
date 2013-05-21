#include "stdafx.h"

#include <algorithm>

#include "Multi_Fuzzy_Line_Intersect_Filter.h"

using namespace std;

pair<vector<building>, vector<building> > Multi_Fuzzy_Line_Intersect_Filter::multi_fuzzy_line_intersect(building object1,  vector<building> generation_objects, float fuzziness, vector<building> grids)
{
	vector<building> eliminate_list;
	vector<building> valid_list;

	for (vector<building>::iterator it = generation_objects.begin();it != generation_objects.end();it++)
	{
		pair<vector<building>, vector<building> > lists = this->fuzzy_line_intersect(object1, *it, fuzziness, grids);

		eliminate_list.insert(eliminate_list.end(), lists.first.begin(), lists.first.end());
	}

	sort(eliminate_list.begin(), eliminate_list.end(), &Multi_Fuzzy_Line_Intersect_Filter::buildings_are_greater);
	eliminate_list.erase(unique(eliminate_list.begin(), eliminate_list.end(), &Multi_Fuzzy_Line_Intersect_Filter::buildings_are_the_same), eliminate_list.end());

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
			eliminate_list.push_back(*it);
	}

	pair<vector<building>, vector<building> > result;
	result.first = eliminate_list;
	result.second = valid_list;

	return result;
}

bool Multi_Fuzzy_Line_Intersect_Filter::buildings_are_the_same(building i, building j)
{
	return (i.building_position == j.building_position &&
		i.building_size == j.building_size);
}

bool Multi_Fuzzy_Line_Intersect_Filter::buildings_are_greater(building i, building j)
{
	return (i.building_position < j.building_position);
}