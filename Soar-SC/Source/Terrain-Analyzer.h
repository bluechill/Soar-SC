#ifndef TERRAIN_ANALYZER_H
#define TERRAIN_ANALYZER_H 1

#include "sml_Client.h"
#include "SDL/SDL_Thread.h"

class TerrainAnalyzer
{
public:
	TerrainAnalyzer(const std::vector<std::vector<bool> > &map, sml::Agent* agent, SDL_mutex* mu);
	~TerrainAnalyzer();

	void analyze();

	void mapping_function();

private:
	std::vector<std::vector<bool> > map;

	SDL_Thread *thread;
	SDL_mutex *mu;

	SDL_mutex *terrain_mu;
	bool should_die;

	sml::Agent* agent;

	typedef struct {
		int x, y;
		int size_x, size_y;
	
		char c;
	} SVS_Rectangle;

	//Functions
	bool rectangle_contains(const int x,const int y,std::vector<SVS_Rectangle> &rectangles);
	SVS_Rectangle get_rectangle(const int x, const int y,std::vector<SVS_Rectangle> &rectangles);
	void generate_rectangle(const int x_start,const int y_start,std::vector<std::vector<bool> > &map, std::vector<SVS_Rectangle> &rectangles);
};

#endif
