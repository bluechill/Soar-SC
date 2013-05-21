#ifndef Choose_Filter_h
#define Choose_Filter_h

#include "Filter_Common.h"

#include <vector>

class Choose_Filter
{
public:
	Choose_Filter() {}
	~Choose_Filter() {}

	//first = eliminate-list
	//second = valid-list
	std::pair<std::vector<building>, std::vector<building> > choose_within_distance_of(std::vector<building> grid, building within, float distance);
};

#endif
