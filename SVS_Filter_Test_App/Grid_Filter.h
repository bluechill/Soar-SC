#ifndef Grid_Filter_h
#define Grid_Filter_h 1

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>

#include <ctime>

using namespace std;

#include "Filter_Common.h"

class Grid_Filter
{
public:
	Grid_Filter();
	~Grid_Filter();

	vector<building> generate_complete_grid(vector<building> building_locations);

private:
	vector<building> generate_grid(pos location, size grid_size, int tile_buffer, size shape);

	static bool buildings_are_the_same(building i, building j);
	static bool buildings_are_greater(building i, building j);
};

#endif
