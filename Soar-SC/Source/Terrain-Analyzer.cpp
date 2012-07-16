#include "Terrain-Analyzer.h"

#include "Soar_Link.h"

#include <sstream>

using namespace std;
using namespace sml;

int thread_runner(void* data)
{
	TerrainAnalyzer* This = reinterpret_cast<TerrainAnalyzer*>(data);
	This->mapping_function();

	return 0;
}

TerrainAnalyzer::TerrainAnalyzer(const std::vector<std::vector<bool> > &map, sml::Agent* agent, SDL_mutex* mu)
{
	this->map = map;

	this->agent = agent;

	this->mu = mu;
	this->terrain_mu = SDL_CreateMutex();

	this->should_die = false;
}

TerrainAnalyzer::~TerrainAnalyzer()
{
	if (thread)
	{
		SDL_WaitThread(thread,NULL);
		thread = NULL;
	}

	if (terrain_mu)
	{
		SDL_DestroyMutex(terrain_mu);
		terrain_mu = NULL;
	}
}

void TerrainAnalyzer::analyze()
{
	thread = SDL_CreateThread(thread_runner, this);
}

bool TerrainAnalyzer::rectangle_contains(const int x,const int y,vector<SVS_Rectangle> &rectangles)
{
	SVS_Rectangle rect = get_rectangle(x,y,rectangles);
	
	if (rect.x == -1)
		return false;

	return true;
}

TerrainAnalyzer::SVS_Rectangle TerrainAnalyzer::get_rectangle(const int x, const int y,vector<SVS_Rectangle> &rectangles)
{
	for (vector<SVS_Rectangle>::iterator it = rectangles.begin();it != rectangles.end();it++)
	{
		if (it->x <= x && x < it->x + it->size_x &&
			it->y <= y && y < it->y + it->size_y)
			return *it;
	}
	
	SVS_Rectangle rect;
	rect.x = -1;
	rect.y = -1;
	rect.size_x = -1;
	rect.size_y = -1;
	
	return rect;
}

void TerrainAnalyzer::generate_rectangle(const int x_start,const int y_start,vector<vector<bool> > &map, vector<SVS_Rectangle> &rectangles)
{
	vector<int> xs;
	vector<int> ys;
	
	for (size_t y = y_start;y < map.size();y++)
	{
		if (map[y][x_start] || rectangle_contains(x_start, y, rectangles))
			break;

		for (size_t x = x_start;x < map[y].size();x++)
		{
			if (map[y][x] || rectangle_contains(x, y, rectangles))
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
		if (map[y_start][x] || rectangle_contains(x, y_start, rectangles))
			break;
		
		for (size_t y = y_start;y < map.size();y++)
		{
			if (map[y][x] || rectangle_contains(x, y, rectangles))
			{
				ys.push_back(y-y_start);
				break;
			}
		}
	}

	int min_y = map.size() + 1;
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
	
	if (rect.size_x > 10)
		cout << "Invalid rect!" << endl;

	rectangles.push_back(rect);
}
	
void TerrainAnalyzer::mapping_function()
{
	vector<SVS_Rectangle> rectangles;

	vector<bool>* start_y = &map[0];

	size_t map_size = map.size();

	SDL_mutexP(terrain_mu);
	for (size_t y = 0;y < map_size && !should_die;y++,start_y++)
	{
		for (size_t x = 0;x < (*start_y).size() && !should_die;x++)
		{
			SDL_mutexV(terrain_mu);

			if (start_y->at(x) || rectangle_contains(x, y, rectangles))
				continue;

			generate_rectangle(x,y,map,rectangles);

			SDL_mutexP(terrain_mu);
		}
	}
	SDL_mutexV(terrain_mu);

	stringstream ss;

	SDL_mutexP(terrain_mu);

	int i = 0;
	for (vector<SVS_Rectangle>::iterator it = rectangles.begin();it != rectangles.end() && !should_die;i++, it++)
	{
		SDL_mutexV(terrain_mu);

		ss << i;
		string id = ss.str();
		ss.str("");

		SVS_Rectangle rect = *it;

		ss << (float)rect.x/4.0f << " " << Soar_Link::flip_one_d_point((float)rect.y/4.0f, false);
		string pos = ss.str();
		ss.str("");
		
		ss << (float)rect.size_x/4.0f << " " << (float)rect.size_y/4.0f;
		string size = ss.str();
		ss.str("");

		string svs_command = "a rect" + id + " world v " + Soar_Link::unit_box_verts + " p " + pos + " 0 s " + size + " 1";
		cout << "SVS-Actual: " << svs_command << endl;

		SDL_mutexP(mu);
		agent->SendSVSInput(svs_command);
		SDL_mutexV(mu);

		SDL_mutexP(terrain_mu);
	}
	SDL_mutexV(terrain_mu);
}