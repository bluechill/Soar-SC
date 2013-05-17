// SVS_Grid_Filter_Test_App.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>

#include <ctime>

using namespace std;

static const int build_tile_size = 4;

typedef pair<int,int> pos;
typedef pos size;

typedef struct {
	pos building_position;
	size building_size;
} building;

vector<building> generate_grid(pos location, size grid_size, int tile_buffer, size shape)
{
	vector<building> grid;

	for (int y = (location.second - tile_buffer - grid_size.second + 1);y < (location.second + shape.second - 1 + tile_buffer + grid_size.second - 1*(grid_size.second-1));y++)
	{
		for (int x = (location.first - tile_buffer - grid_size.first + 1);x < (location.first + shape.first - 1 + tile_buffer + grid_size.first - 1*(grid_size.first-1));x++)
		{
			building temp_building;

			temp_building.building_position = make_pair<int,int>(x,y);
			temp_building.building_size = grid_size;

			grid.push_back(temp_building);
		}
	}

	////Left Side Corners
	//grid.push_back(make_pair<int,int>(location.first - tile_buffer, location.second + shape.second - 1 + tile_buffer));
	//grid.push_back(make_pair<int,int>(location.first - tile_buffer, location.second - tile_buffer));

	////Right Side Corners
	//grid.push_back(make_pair<int,int>(location.first + shape.first - 1 + tile_buffer, location.second + shape.second - 1 + tile_buffer));
	//grid.push_back(make_pair<int,int>(location.first + shape.first - 1 + tile_buffer, location.second - tile_buffer));

	//for (int i = 0;i < shape.second;i += (shape.second % 2 == 0 ? 3 : 2))
	//{
	//	grid.push_back(make_pair<int,int>(location.first - tile_buffer, location.second + i));
	//	grid.push_back(make_pair<int,int>(location.first + shape.first - 1 + tile_buffer, location.second + i));
	//}

	//for (int i = 0;i < shape.first;i += (shape.first % 2 == 0 ? 3 : 2))
	//{
	//	grid.push_back(make_pair<int,int>(location.first + i, location.second - tile_buffer));
	//	grid.push_back(make_pair<int,int>(location.first + i, location.second + shape.second - 1 + tile_buffer));
	//}

	return grid;
}

bool buildings_are_the_same(building i, building j)
{
	return (i.building_position == j.building_position &&
		i.building_size == j.building_size);
}

bool buildings_are_greater(building i, building j)
{
	return (i.building_position < j.building_position);
}

int main(int argc, char* argv[])
{
	srand(size_t(time(NULL)));

	vector<building> building_locations;
	/*
	G  G  G  G  G
	       
	G  B  B  B  G

	G  B  B  G  G
	
	G  B  G  G    

	G  G  G
	*/

	/*pos base_start = make_pair<int,int>(5,5);

	for (int y = base_start.second;y < 15;y += 2)
	{
		for (int x = base_start.first;x < (15-y);x += 2)
		{
			building new_building;

			new_building.building_position = make_pair<int,int>(x, y);

			int size_x = rand()%3 + 1;
			int size_y = rand()%3 + 1;

			new_building.building_size = make_pair<int,int>(size_x,size_y);

			if (size_x > 1)
				x += (size_x - 1);

			if (size_y > 1)
				y += (size_y - 1);

			building_locations.push_back(new_building);
		}
	}*/

	building temp_building;

	temp_building.building_position = make_pair<int,int>(5,5);
	temp_building.building_size = make_pair<int,int>(3,3);
	building_locations.push_back(temp_building);

	temp_building.building_position = make_pair<int,int>(9,5);
	temp_building.building_size = make_pair<int,int>(2,1);
	building_locations.push_back(temp_building);
	
	temp_building.building_position = make_pair<int,int>(5,9);
	temp_building.building_size = make_pair<int,int>(2,1);
	building_locations.push_back(temp_building);

	vector<building> final_grid;

	for (size_t i = 0;i < building_locations.size();i++)
	{
		vector<building> grid = generate_grid(building_locations[i].building_position, make_pair<int,int>(1,1), 2, building_locations[i].building_size);

		final_grid.insert(final_grid.end(), grid.begin(), grid.end());
	}

	//Remove duplicates
	sort(final_grid.begin(), final_grid.end(), buildings_are_greater);
	final_grid.erase(unique(final_grid.begin(), final_grid.end(), buildings_are_the_same), final_grid.end());

	//Remove building locations as free
	for (size_t i = 0;i < building_locations.size();i++)
	{
		int building_left_edge = building_locations[i].building_position.first;
		int building_right_edge = building_locations[i].building_position.first + building_locations[i].building_size.first;

		int building_top_edge = building_locations[i].building_position.second;
		int building_bottom_edge = building_locations[i].building_position.second + building_locations[i].building_size.second;

		for (vector<building>::iterator it = final_grid.begin();it != final_grid.end();)
		{
			int grid_left_edge = it->building_position.first;
			int grid_right_edge = it->building_position.first + it->building_size.first;

			int grid_top_edge = it->building_position.second;
			int grid_bottom_edge = it->building_position.second + it->building_size.second;

			if (grid_right_edge < building_left_edge ||
				grid_left_edge > building_right_edge ||
				grid_bottom_edge < building_top_edge ||
				grid_top_edge > building_bottom_edge ) //No collision exists, no overlap
			{
				it++;
			}
			else
				it = final_grid.erase(it);
		}
	}

	//Output a fancy grid to a file to show the results
	ofstream file("svs_grid_filter_output_test.txt");

	if (!file)
	{
		cerr << "Could not open file 'svs_grid_filter_output_test.txt'" << endl;
		return 1;
	}

	vector<vector<char> > file_to_output;

	for (int i = 0;i < 64;i++)
	{
		vector<char> line;

		for (int j = 0;j < 64;j++)
			line.push_back(' ');

		file_to_output.push_back(line);
	}

	for (size_t i = 0;i < building_locations.size();i++)
	{
		for (int y = 0;y < building_locations[i].building_size.second;y++)
		{
			for (int x = 0;x < building_locations[i].building_size.first;x++)
				file_to_output[building_locations[i].building_position.second + y][building_locations[i].building_position.first + x] = 'B';
		}
	}

	for (size_t i = 0;i < final_grid.size();i++)
	{
		for (int y = 0;y < final_grid[i].building_size.second;y++)
		{
			for (int x = 0;x < final_grid[i].building_size.first;x++)
				file_to_output[final_grid[i].building_position.second + y][final_grid[i].building_position.first + x] = 'G';
		}
	}

	for (size_t y = 0;y < file_to_output.size();y++)
	{
		for (size_t x = 0;x < file_to_output[y].size();x++)
			file << file_to_output[y][x];

		file << endl;
	}

	return 0;
}

