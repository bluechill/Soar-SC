#ifndef TERRAIN_ANALYZER_H //Make sure this header is only included once to prevent duplicates
#define TERRAIN_ANALYZER_H 1

#include "sml_Client.h" //For SML Agent (Soar) stuff
#include "SDL/SDL_Thread.h" //For SDL Threading

#include <iostream> //For io functions and classes like cout cerr etc.

#include <fstream> //For file functions and classes like fstream

#include <vector> //For std::vector class

class TerrainAnalyzer //Class for analyzing a terrain and sending everything to SVS
{
public:
	TerrainAnalyzer(const std::vector<std::vector<bool> > &map, sml::Agent* agent, SDL_mutex* mu); //Constructor, takes a vector of a vector of bools as to whether a single tile is walkable or not.  Also takes a pointer to the Soar Agent and a pointer to the SDL mutex used by the Soar Link class
	~TerrainAnalyzer(); //Deconstructor

	void analyze(); //Start analyzing the map.  Creates a thread to do all the work.

	void mapping_function(); //Function for analyzing the map

	bool done_sending_svs(); //Acknowledge being done.  Returns whether the analyzer has analyzed the map.

private:
	std::ofstream out; //Output to a file the terrain SVS commands

	std::vector<std::vector<bool> > map; //Internal variable for the map

	SDL_Thread *thread; //Thread variable for the analyzer thread
	SDL_mutex *mu; //Soar Link mutex

	bool should_die; //Set to true to kill the analyzer thread if it's still running.

	sml::Agent* agent; //Pointer to the Soar agent

	typedef struct {
		int x, y;
		int size_x, size_y;
	
		char c;
	} SVS_Rectangle; //struct for holding a rectangle to be sent to SVS

	//Functions
	bool rectangle_contains(const int x,const int y,std::vector<SVS_Rectangle> &rectangles); //Check whether a rectangle contains a point
	SVS_Rectangle get_rectangle(const int x, const int y,std::vector<SVS_Rectangle> &rectangles); //Get the rectangle at the point given
	void generate_rectangle(const int x_start,const int y_start,std::vector<std::vector<bool> > &map, std::vector<SVS_Rectangle> &rectangles); //Generate a rectangle based on a start point and the rest of the map.

	bool done_svs; //Whether the thread (analyzer) is done yet.
};

#endif
