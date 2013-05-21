#ifndef Multi_Fuzzy_Line_Intersect_Filter_h
#define Multi_Fuzzy_Line_Intersect_Filter_h 1

#include "Fuzzy_Line_Intersect_Filter.h"

#include <vector>

class Multi_Fuzzy_Line_Intersect_Filter : private Fuzzy_Line_Intersect_Filter
{
public:
	Multi_Fuzzy_Line_Intersect_Filter() {}
	~Multi_Fuzzy_Line_Intersect_Filter() {}

	std::pair<std::vector<building>, std::vector<building> > multi_fuzzy_line_intersect(building object1, std::vector<building> generation_objects, float fuzziness, std::vector<building> grids);
private:
	static bool buildings_are_the_same(building i, building j);
	static bool buildings_are_greater(building i, building j);
};

#endif
