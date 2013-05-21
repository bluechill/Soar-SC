#ifndef Multi_Choose_Filter_h
#define Multi_Choose_Filter_h 1

#include <vector>

#include "Filter_Common.h"
#include "Choose_Filter.h"

class Multi_Choose_Filter : private Choose_Filter
{
public:
	Multi_Choose_Filter() {}
	~Multi_Choose_Filter() {}

	//first = eliminate-list
	//second = valid-list
	std::pair<std::vector<building>, std::vector<building> > multi_choose_within_distance_of(std::vector<building> grid, std::vector<building> within, float distance);

private:
	static bool buildings_are_the_same(building i, building j);
	static bool buildings_are_greater(building i, building j);
};

#endif
