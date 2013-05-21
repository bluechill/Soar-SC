#include "stdafx.h"

#include "Fuzzy_Line_Intersect_Filter.h"

#include <cmath>

using namespace std;

pair<vector<building>, vector<building> > Fuzzy_Line_Intersect_Filter::fuzzy_line_intersect(building object1, building object2, float fuzziness, vector<building> grids)
{
	vector<building> eliminate_list;
	vector<building> valid_list;

	//Generate the rotated rectangle for doing collision tests
	Rotated_Rectangle line_rect;

	//Average value between the two buildings is the center
	line_rect.center.x = float((object2.building_position.first + object1.building_position.first)/2);
	line_rect.center.y = float((object2.building_position.second + object1.building_position.second)/2);

	//Absolute value of the difference between the two buildings is the size
	line_rect.size.x = float(abs(object2.building_position.first - object1.building_position.first));
	line_rect.size.y = float(abs(object2.building_position.second - object1.building_position.second));

	//1 = Top Right
	//2 = Top Left
	//3 = Bottom Left
	//4 = Bottom Right
	int quadrant = 1;

	if (object1.building_position.second > object2.building_position.second)
	{
		//Bottom Quadrants
		if (object1.building_position.first > object2.building_position.first)
			quadrant = 3; //Bottom Left
		else
			quadrant = 4; //Bottom Right
	}
	else
	{
		//Top Quadrants
		if (object1.building_position.first > object2.building_position.first)
			quadrant = 2; // Top Left Quadrant
		else
			quadrant = 1; // Top Right Quadrant
	}

	float angle_addition = 0.0f;
	
	const float pi = atan(1.0f)*4.0f; // tan inverse of 1 is pi/4

	//sin = opposite / hypo
	//opposite = y in this case
	float y = line_rect.size.y;
	
	//Calculate the abs hypo
	float x = line_rect.size.x;

	float hypo = sqrt(pow(x, 2) + pow(y, 2));

	//Nothing to do for Quadrant 1
	if (quadrant == 2)
		angle_addition = pi/2;
	else if (quadrant == 3)
		angle_addition = pi;
	else if (quadrant == 4)
		angle_addition = 3/2 * pi;

	float angle = asinf(y/hypo) + angle_addition;

	line_rect.angle = angle;

	//Compensate for the fuzziness
	line_rect.size.x += 2*cos(angle - pi/2);
	line_rect.size.y += 2*sin(angle - pi/2);

	//Calculate all intersects
	for (vector<building>::iterator it = grids.begin();it != grids.end();it++)
	{
		Rotated_Rectangle grid;
		grid.angle = 0.0f;
		grid.center.x = float(it->building_position.first + it->building_size.first/2);
		grid.center.y = float(it->building_position.second + it->building_size.second/2);

		grid.size.x = float(it->building_size.first);
		grid.size.y = float(it->building_size.second);

		if (Rotated_Rectangle_Collision(&line_rect, &grid))
			eliminate_list.push_back(*it);
		else
			valid_list.push_back(*it);
	}

	pair<vector<building>, vector<building> > result;
	result.first = eliminate_list;
	result.second = valid_list;

	return result;
}

void Fuzzy_Line_Intersect_Filter::Add_Vectors2D(Vector2D *v1, Vector2D *v2)
{
	v1->x += v2->x;
	v1->y += v2->y;
}

void Fuzzy_Line_Intersect_Filter::Subtract_Vectors2D(Vector2D *v1, Vector2D *v2)
{
	v1->x -= v2->x;
	v1->y -= v2->y;
}

void Fuzzy_Line_Intersect_Filter::Rotate_Vector2D_Clockwise(Vector2D *v, float angle)
{
	float vector_x = v->x;
	float cos_angle = cos(angle);
	float sin_angle = sin(angle);

	v->x = vector_x * cos_angle + v->y * sin_angle;
	v->y = -vector_x * sin_angle + v->y * cos_angle;
}

bool Fuzzy_Line_Intersect_Filter::Rotated_Rectangle_Collision(Rotated_Rectangle *rect1, Rotated_Rectangle *rect2)
{
	//Copy the given variables so we don't touch them
	Rotated_Rectangle rectangle_1 = *rect1;
	Rotated_Rectangle rectangle_2 = *rect2;

	//Translate the plane by the negative of rectangle 1's center
	//Now rectangle 1 is at the origin
	Subtract_Vectors2D(&rectangle_1.center, &rectangle_1.center);
	Subtract_Vectors2D(&rectangle_2.center, &rectangle_1.center);

	//Rotate the plane by the negative of rectangle 2's angle
	//Now rectangle 2 is axis-aligned and rectangle 1 is at the origin
	Rotate_Vector2D_Clockwise(&rectangle_1.center, -rectangle_2.angle);
	Rotate_Vector2D_Clockwise(&rectangle_2.center, -rectangle_2.angle);
	
	rectangle_1.angle -= rectangle_2.angle;
	rectangle_2.angle -= rectangle_2.angle;

	//Calculate the vertices of	the now rotated rectangle 2
	Vector2D bottom_left;
	Vector2D top_right;

	bottom_left = rectangle_1.center;
	top_right = rectangle_1.center;

	Subtract_Vectors2D(&bottom_left, &rectangle_1.size);
	Add_Vectors2D(&top_right, &rectangle_1.size);

	//Since Rectangle 1 is centered on the origin, we can define two pairs
	//of vertices, A and B, such that A is the vertical minimum and -A is
	//the vertical max and B is the horizontal minimum and -B is the
	//horizontal maximum

	Vector2D A;
	Vector2D B;

	float sin_angle = sin(rectangle_1.angle);
	float cos_angle = cos(rectangle_1.angle);

	A.x = -rectangle_1.size.y * sin_angle;
	B.x = A.x;
	
	float t = rectangle_1.size.x * cos_angle;
	A.x += t;
	B.x -= t;

	A.y = rectangle_1.size.y * cos_angle;
	B.y = A.y;
	
	t = rectangle_1.size.x * sin_angle;
	A.y += t;
	B.y -= t;

	t = sin_angle * cos_angle;

	//Make sure A is the vert min/max and B is the horiz min/max
	if (t < 0)
	{
		t = A.x;
		A.x = B.x;
		B.x = t;
		
		t = A.y;
		A.y = B.y;
		B.y = t;
	}

	//Make sure B is the horiz min
	if (sin_angle < 0)
	{
		B.x = -B.x;
		B.y = -B.y;
	}

	//If rectangle 2 isn't between the left and right of rectangle 1, collision is impossible
	if (B.x > top_right.x ||
		B.x > -bottom_left.x)
	{
		return false;
	}

	float rect1_vert_min;
	float rect1_vert_max;

	//If rectangle 1 is axis-aligned as well, then we can easily find it's vert min and max
	if (t == 0) //t is sin_angle * cos_angle
	{
		rect1_vert_min = A.y;
		rect1_vert_max = -A.y;
	}
	else
	{
		float x = bottom_left.x - A.x;
		float a = top_right.x - A.x;

		rect1_vert_min = A.y;
		//If the vertical min/max isn't on the range of the rectangle (cross rectangle check),
		//then find it on that range (has to be at bottom_left.x or top_right.x)
		if (a * x > 0)
		{
			float dx = A.x;
			if (x < 0)
			{
				dx -= B.x;
				rect1_vert_min -= B.y;
				x = a;
			}
			else
			{
				dx += B.x;
				rect1_vert_min += B.y;
			}

			rect1_vert_min *= x;
			rect1_vert_min /= dx;
			rect1_vert_min += A.y;
		}

		x = bottom_left.x + A.x;
		a = top_right.x + A.x;

		rect1_vert_max = -A.y;
		// if the second vertical min/max isn't in (BL.x, TR.x), then
		// find the local vertical min/max on BL.x or on TR.x
		if (a*x > 0)
		{
			float dx = -A.x;
			if (x < 0)
			{
				dx -= B.x;
				rect1_vert_max -= B.y;
				x = a;
			}
			else
			{
				dx += B.x;
				rect1_vert_max += B.y;
			}

			rect1_vert_max *= x;
			rect1_vert_max /= dx;
			rect1_vert_max -= A.y;
		}
	}

	//Return true if the vert max or vert min is within rect 2
	if (rect1_vert_min < bottom_left.y &&
		rect1_vert_max < bottom_left.y)
		return false;
	else if (rect1_vert_min > top_right.y &&
			 rect1_vert_max > top_right.y)
		return false;

	return true;
}
