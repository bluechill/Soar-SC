#include <BWAPI.h>
#include "Polygon.h"

using namespace BWAPI;
using namespace std;

bool Polygon::contains_position(Position pos)
{
	for (vector<Position>::iterator it = this->begin();it != this->end();it++)
	{
		if ((*it) == pos)
			return true;
	}

	return false;
}
