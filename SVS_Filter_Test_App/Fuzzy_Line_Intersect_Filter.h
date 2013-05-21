#ifndef Fuzzy_Line_Intersect_h
#define Fuzzy_Line_Intersect_h 1

#include <utility>
#include <vector>

#include "Filter_Common.h"

typedef struct {
	float x;
	float y;
} Vector2D;

typedef	struct {
	Vector2D center; // (X,Y) 
	Vector2D size; // (width, height)
	float angle; //Angle rotated by
} Rotated_Rectangle;

class Fuzzy_Line_Intersect_Filter
{
public:
	Fuzzy_Line_Intersect_Filter() {}
	~Fuzzy_Line_Intersect_Filter() {}

	//First = eliminate_list
	//Second = valid_list
	std::pair<std::vector<building>, std::vector<building> > fuzzy_line_intersect(building object1, building object2, float fuzziness, std::vector<building> grids);

private:
	//Rotated Rectangle Code
	//Method from http://www.ragestorm.net/tutorial?id=22

	inline void Add_Vectors2D(Vector2D *v1, Vector2D *v2);
	inline void Subtract_Vectors2D(Vector2D *v1, Vector2D *v2);

	inline void Rotate_Vector2D_Clockwise(Vector2D *v, float angle);

	bool Rotated_Rectangle_Collision(Rotated_Rectangle *rect1, Rotated_Rectangle *rect2);
};

#endif
