#include "Soar_Link.h" //Include Soar Link class header

#include <string>

#include <windows.h>
#include <iostream>

using namespace BWAPI; //Use namespaces to allow use of string instead of std::string or Agent* instead of sml::Agent* for example
using namespace std;
using namespace sml;

const std::string Soar_Link::unit_box_verts = "0 0 0 0 0 1 0 1 0 0 1 1 1 0 0 1 0 1 1 1 0 1 1 1"; //The vertices of a 1x1x1 unit box

float Soar_Link::flip_one_d_point(const float &point, const bool &x_axis) //Flip a point around the map's half point in the x or y axis
{
	int map;

	if (x_axis)
		map = Broodwar->mapWidth();
	else
		map = Broodwar->mapHeight();

	float flipped_point = point;
	//Convert the point to be bewtween 0 and 1
	flipped_point /= map;
	//Subtract 0.5 so 0.5 in the point becomes 0
	flipped_point -= 0.5f;
	//Multiply by -1 to flip it around the axis
	flipped_point *= -1;
	//Add 0.5 again so it's from 0 to 1 again
	flipped_point += 0.5f;
	//Multiple by the map size in the axis to convert back to the point
	flipped_point *= map;

	//Return it
	return flipped_point;
}
