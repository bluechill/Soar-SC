// Soar-SC-Rectilinear.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>

#include <vector>
#include <sstream>

#include <fstream>

#include <ctime>

using namespace std;

typedef struct {
	int x, y;
	int size_x, size_y;
	
	char c;
} SVS_Rectangle;

bool isWalkable(char c)
{
	if (c == '.')
		return true;
	
	return false;
}

bool rectangle_contains(const int x,const int y,const vector<SVS_Rectangle> &rectangles)
{
	for (size_t i = 0;i < rectangles.size();i++)
	{
		SVS_Rectangle rect = rectangles[i];
		if (rect.x <= x && x < rect.x + rect.size_x &&
			rect.y <= y && y < rect.y + rect.size_y)
			return true;
	}
	
	return false;
}

SVS_Rectangle get_rectangle(const int x, const int y,const vector<SVS_Rectangle> &rectangles)
{
	for (size_t i = 0;i < rectangles.size();i++)
	{
		SVS_Rectangle rect = rectangles[i];
		if (rect.x <= x && x < rect.x + rect.size_x &&
			rect.y <= y && y < rect.y + rect.size_y)
			return rect;
	}
	
	SVS_Rectangle rect;
	rect.x = -1;
	rect.y = -1;
	rect.size_x = -1;
	rect.size_y = -1;
	
	return rect;
}

void generate_rectangle(const int x_start,const int y_start,const vector<string> &map, vector<SVS_Rectangle> &rectangles)
{
	vector<size_t> xs;
	vector<size_t> ys;
	
	for (size_t y = y_start;y < map.size();y++)
	{
		if (isWalkable(map[y][x_start]) || rectangle_contains(x_start, y, rectangles))
			break;

		for (size_t x = x_start;x < map[y].size();x++)
		{
			if (isWalkable(map[y][x]) || rectangle_contains(x, y, rectangles))
			{
				xs.push_back(x-x_start);
				break;
			}
		}
	}
	
	size_t min_x = map[0].size() + 1;
	for (size_t i = 0;i < xs.size();i++)
	{
		if (xs[i] < min_x)
			min_x = xs[i];
	}
	
	for (size_t x = x_start;x < min_x + x_start && x < map.size();x++)
	{
		if (isWalkable(map[y_start][x]) || rectangle_contains(x, y_start, rectangles))
			break;
		
		for (size_t y = y_start;y < map.size();y++)
		{
			if (isWalkable(map[y][x]) || rectangle_contains(x, y, rectangles))
			{
				ys.push_back(y-y_start);
				break;
			}
		}
	}

	size_t min_y = map.size() + 1;
	for (size_t i = 0;i < ys.size();i++)
	{
		if (ys[i] < min_y)
			min_y = ys[i];
	}
	
	SVS_Rectangle rect;
	rect.x = x_start;
	rect.y = y_start;
	rect.size_x = min_x;
	rect.size_y = min_y;
	
	rectangles.push_back(rect);
}

int main(int argc, const char * argv[])
{
	if (argc != 2)
	{
		cout << "Usage: " << argv[0] << " <file-input>" << endl;
		cout << "Where <file-input> is the path (relative or absolute) to the input file of A because open and B being closed" << endl;
		return 1;
	}
	
	vector<string> file_lines;
	
	ifstream in(argv[1], ios::in);
	
	if (!in.is_open())
	{
		cout << "Could not open file: " << argv[1] << endl;
		return 3;
	}

	string line;
	while (getline(in, line))
		file_lines.push_back(line);

	int y_size = file_lines.size();
	int x_size = file_lines[0].size();
	for (size_t y = 0;y < file_lines.size();y++)
	{
		if (file_lines[y].size() != x_size)
		{
			cerr << "ERROR! Line sizes do not match!" << endl;
			return 5;
		}
	}
	
//	for (int y = 0;y < file_lines.size();y++)
//	{
//		for (int x = 0;x < file_lines[y].size();x++)
//			cout << file_lines[y][x];
//		
//		cout << endl;
//	}
	
	vector<SVS_Rectangle> rectangles;
	
	clock_t start = clock();
	
	string* start_y = &file_lines[0];
	for (size_t y = 0;y < file_lines.size();y++,start_y++)
	{
		char* start_x = &(*start_y)[0];
		for (size_t x = 0;x < (*start_y).size();x++,start_x++)
		{
			if (isWalkable(*start_x) || rectangle_contains(x, y, rectangles))
				continue;

			generate_rectangle(x,y,file_lines,rectangles);
		}
	}
	
	clock_t end = clock();
	
	cout << "Time: " << ((float)end - (float)start)/CLOCKS_PER_SEC << "s" << endl;
	
	cout << endl;
	
	char c = '0';
	cout << "Rectangles: " << rectangles.size() << endl;
	for (size_t i = 0;i < rectangles.size();i++)
	{
		rectangles[i].c = c;
		c++;
	}
	
	for (size_t y = 0;y < file_lines.size();y++)
	{
		for (size_t x = 0;x < file_lines[y].size();x++)
		{
			SVS_Rectangle rect = get_rectangle(x, y, rectangles);
			if (rect.x == -1)
				cout << ".";
			else
				cout << rect.c;
		}
		cout << endl;
	}
	
    return 0;
}


