#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <BWAPI.h>

class Polygon : public std::vector<BWAPI::Position>
{
public:
	bool contains_position(BWAPI::Position pos);
};

#endif
