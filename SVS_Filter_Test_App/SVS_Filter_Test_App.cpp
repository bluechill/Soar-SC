// SVS_Grid_Filter_Test_App.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>

#include <ctime>

#include "Filter_Common.h"
#include "Grid_Filter.h"

using namespace std;

int main(int argc, char* argv[])
{
	srand(size_t(time(NULL)));

	vector<building> building_locations;
	
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

	Grid_Filter generate_grid;

	vector<building> final_grid = generate_grid.generate_complete_grid(building_locations);

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

