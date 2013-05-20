#ifndef Filter_Common_h
#define Filter_Common_h 1

#include <vector>
#include <map>

static const int build_tile_size = 4;

typedef std::pair<int,int> pos;
typedef pos size;

typedef struct {
	pos building_position;
	size building_size;
} building;

#endif
