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

vector<pos> generate_grid(pos location, int tile_buffer, size shape)
{
	vector<pos> grid;

	//Left Side Corners
	grid.push_back(make_pair<int,int>(location.first - tile_buffer, location.second + shape.second - 1 + tile_buffer));
	grid.push_back(make_pair<int,int>(location.first - tile_buffer, location.second - tile_buffer));

	//Right Side Corners
	grid.push_back(make_pair<int,int>(location.first + shape.first - 1 + tile_buffer, location.second + shape.second - 1 + tile_buffer));
	grid.push_back(make_pair<int,int>(location.first + shape.first - 1 + tile_buffer, location.second - tile_buffer));

	for (int i = 0;i < shape.second;i += (shape.second % 2 == 0 ? 3 : 2))
	{
		grid.push_back(make_pair<int,int>(location.first - tile_buffer, location.second + i));
		grid.push_back(make_pair<int,int>(location.first + shape.first - 1 + tile_buffer, location.second + i));
	}

	for (int i = 0;i < shape.first;i += (shape.first % 2 == 0 ? 3 : 2))
	{
		grid.push_back(make_pair<int,int>(location.first + i, location.second - tile_buffer));
		grid.push_back(make_pair<int,int>(location.first + i, location.second + shape.second - 1 + tile_buffer));
	}

	return grid;
}

int main(int argc, char* argv[])
{
	srand(time(NULL));

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

	vector<pos> final_grid;

	for (int i = 0;i < building_locations.size();i++)
	{
		vector<pos> grid = generate_grid(building_locations[i].building_position, 2, building_locations[i].building_size);

		final_grid.insert(final_grid.end(), grid.begin(), grid.end());
	}

	//Remove duplicates
	sort(final_grid.begin(), final_grid.end());
	final_grid.erase(unique(final_grid.begin(), final_grid.end()), final_grid.end());

	//Remove building locations as free
	for (int i = 0;i < building_locations.size();i++)
	{
		int left_edge = building_locations[i].building_position.first - 1;
		int right_edge = building_locations[i].building_position.first + building_locations[i].building_size.first;

		int top_edge = building_locations[i].building_position.second - 1;
		int bottom_edge = building_locations[i].building_position.second + building_locations[i].building_size.second;

		for (vector<pos>::iterator it = final_grid.begin();it != final_grid.end();)
		{
			if (it->first >= left_edge		&&
				it->first <= right_edge		&&
				it->second >= top_edge		&&
				it->second <= bottom_edge	)
			{
				it = final_grid.erase(it);
			}
			else
				it++;
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

	for (int i = 0;i < building_locations.size();i++)
	{
		for (int y = 0;y < building_locations[i].building_size.second;y++)
		{
			for (int x = 0;x < building_locations[i].building_size.first;x++)
				file_to_output[building_locations[i].building_position.second + y][building_locations[i].building_position.first + x] = 'B';
		}
	}

	for (int i = 0;i < final_grid.size();i++)
		file_to_output[final_grid[i].second][final_grid[i].first] = 'G';

	for (int y = 0;y < file_to_output.size();y++)
	{
		for (int x = 0;x < file_to_output[y].size();x++)
			file << file_to_output[y][x];

		file << endl;
	}

	return 0;
}

