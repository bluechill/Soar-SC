#include "Soar_Link.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <vector>
#include <set>
#include <sstream>

using namespace BWAPI;
using namespace std;
using namespace sml;

void Soar_Link::onStart()
{
	if (kernel->HadError())
	{
		const char* msg = kernel->GetLastErrorDescription();
		cout << "Soar: " << msg << endl;
		Broodwar->printf("Soar: %s", msg);
		return;
	}

	agent = kernel->CreateAgent("Soar-SC");

	if (kernel->HadError())
	{
		const char* msg = kernel->GetLastErrorDescription();
		cout << "Soar: " << msg << endl;
		Broodwar->printf("Soar: %s", msg);
		return;
	}

	agent->ExecuteCommandLine("source Soar-SC/Soar-SC.soar");

	agent->ExecuteCommandLine("waitsnc -e");

	stringstream ss;
	ss << Broodwar->mapWidth();
	string map_width_as_string = ss.str();
	ss.str("");
	ss << Broodwar->mapHeight();
	string map_height_as_string = ss.str();
	ss.str("");

	//Bottom barrier
	string svs_command_1 = "a -x0 world v " + unit_box_verts + " p 0 -1 0 s " + map_width_as_string + " 1 1";

	test_input_file << "SVS-Actual: " << svs_command_1 << endl;

	agent->SendSVSInput(svs_command_1);

	//Top barrier
	string svs_command_2 = "a x0 world v " + unit_box_verts + " p 0 " + map_height_as_string + " 0 s " + map_width_as_string + " 1 1";

	test_input_file << "SVS-Actual: " << svs_command_2 << endl;

	agent->SendSVSInput(svs_command_2);

	//Left barrier
	string svs_command_3 = "a -y0 world v " + unit_box_verts + " p -1 0 0 s 1 " + map_height_as_string + " 1";

	test_input_file << "SVS-Actual: " << svs_command_3 << endl;

	agent->SendSVSInput(svs_command_3);

	//Right barrier
	string svs_command_4 = "a y0 world v " + unit_box_verts + " p " + map_width_as_string + " 0 0 s 1 " + map_height_as_string + " 1";

	test_input_file << "SVS-Actual: " << svs_command_4 << endl;

	agent->SendSVSInput(svs_command_4);

	cout << "Soar-SC is running." << endl;
	Broodwar->printf("Soar-SC is running.");

	soar_thread = SDL_CreateThread(thread_runner_soar, this);
	if (!soar_thread)
	{
		Broodwar->printf("Soar: Unable to create soar thread!");
		cout << "Soar: Unable to create soar thread!" << endl;
	}

	if (console)
		console_thread = SDL_CreateThread(thread_runner_console, this);

	vector<vector<bool> > map;
	size_t map_size_x = Broodwar->mapWidth() * 4;
	size_t map_size_y = Broodwar->mapHeight() * 4;

	vector<vector<bool> > build_tiles_map;
	
	for (size_t y = 0;y < map_size_y;y++)
	{
		vector<bool> map_y;

		int last_16 = 0;
		for (size_t x = 0;x < map_size_x;x++)
		{
			if (x == 88)
				cout << "80!" << endl;

			map_y.push_back(Broodwar->isWalkable(x,y));
			if (x % 4 == 0)
			{
				if ((x-3) > 0 && !map_y[x-3])
					last_16++;

				if ((x-2) > 0 && !map_y[x-2])
					last_16++;

				if ((x-2) > 0 && !map_y[x-1])
					last_16++;

				if (y != 0 && !map_y[x])
					last_16++;
			}

			if (y % 4 == 0)
			{
				if ((y-3) > 0 && !map[y-3][x])
					last_16++;

				if ((y-2) > 0 && !map[y-2][x])
					last_16++;

				if ((y-1) > 0 && !map[y-1][x])
					last_16++;

				if (!map_y[x])
					last_16++;
			}

			if (y % 4 == 0 && x % 4 == 0 && last_16 > 0)
			{
				if (y == 0 && x == 0)
				{
					map_y[x] = false;

					last_16 = 0;
					continue;
				}
				else if (y == 0)
				{
					for (int j = 3;j >= 0;j--)
						map_y[x-j] = false;

					continue;
				}

				for (int i = 3;i >= 0;i--)
				{
					if (x == 0)
					{
						if (i == 0)
							map_y[x] = false;
						else
							map[y-i][x] = false;

						last_16 = 0;

						continue;
					}

					for (int j = 3;j >= 0;j--)
					{
						if (i == 0)
							map_y[x-j] = false;
						else
							map[y-i][x-j] = false;
					}
				}

				last_16 = 0;
			}
		}

		map.push_back(map_y);
	}

	//Clean up the map even more

	for (size_t y = 0;y < map_size_y;y += 4)
	{
		for (size_t x = 0;x < map_size_x;x += 4)
		{
			if (map[y][x])
			{
				int surrounding = 0;
				
				int distance = 5;

				if (y + distance < map.size() && !map[y+distance][x])
					surrounding++;
				else if (y + distance - 1 < map.size() && !map[y+distance-1][x])
					surrounding++;

				if (y - distance >= 0 && !map[y-distance][x])
					surrounding++;
				else if (y - distance + 1 >= 0 && !map[y-distance+1][x])
					surrounding++;

				if (x + distance < map[y].size() && !map[y][x+distance])
					surrounding++;
				else if (x + distance - 1 < map[y].size() && !map[y][x+distance-1])
					surrounding++;

				if (x - distance >= 0 && !map[y][x-distance])
					surrounding++;
				else if (x - distance + 1 >= 0 && !map[y][x-distance+1])
					surrounding++;

				//Diagonals

				if (y + distance < map.size() && x + distance < map[y+distance].size() && !map[y+distance][x+distance])
					surrounding++;
				else if (y + distance - 1 < map.size() && x + distance < map[y+distance-1].size() && !map[y+distance-1][x+distance])
					surrounding++;
				else if (y + distance - 1 < map.size() && x + distance - 1 < map[y+distance-1].size() && !map[y+distance-1][x+distance-1])
					surrounding++;
				else if (y + distance < map.size() && x + distance - 1 < map[y+distance].size() && !map[y+distance][x+distance-1])
					surrounding++;

				if (y + distance < map.size() && x - distance >= 0 && !map[y+distance][x-distance])
					surrounding++;
				else if (y + distance - 1 < map.size() && x - distance >= 0 && !map[y+distance-1][x-distance])
					surrounding++;
				else if (y + distance - 1 < map.size() && x - distance + 1 >= 0 && !map[y+distance-1][x-distance+1])
					surrounding++;
				else if (y + distance < map.size() && x - distance + 1 >= 0 && !map[y+distance][x-distance+1])
					surrounding++;

				if (y - distance >= 0 && x + distance < map[y-distance].size() && !map[y-distance][x+distance])
					surrounding++;
				else if (y - distance + 1 >= 0 && x + distance < map[y-distance+1].size() && !map[y-distance+1][x+distance])
					surrounding++;
				else if (y - distance + 1 >= 0 && x + distance - 1 < map[y-distance+1].size() && !map[y-distance+1][x+distance-1])
					surrounding++;
				else if (y - distance >= 0 && x + distance - 1 < map[y-distance].size() && !map[y-distance][x+distance-1])
					surrounding++;

				if (y - distance >= 0 && x - distance >= 0 && !map[y-distance][x-distance])
					surrounding++;
				else if (y - distance + 1 >= 0 && x - distance >= 0 && !map[y-distance+1][x-distance])
					surrounding++;
				else if (y - distance + 1 >= 0 && x - distance + 1 >= 0 && !map[y-distance+1][x-distance+1])
					surrounding++;
				else if (y - distance >= 0 && x - distance + 1 >= 0 && !map[y-distance][x-distance+1])
					surrounding++;

				if (surrounding > 4) //Greater than 50%
				{
					for (int i = 3;i >= 0;i--)
					{
						for (int j = 3;j >= 0;j--)
							map[y-i][x-j] = false;
					}
					continue;
				}

				if (surrounding > 2 && x == 96  && y > 100)
					cout << "TEST!" << endl;
			}
		}
	}

	analyzer = new TerrainAnalyzer(map, agent, mu);
	analyzer->analyze();

	/*ofstream ifs("bwapi-data/logs/map-3.txt", ios::out);

	if (!ifs.is_open())
	return;

	for (int y = 0;y < map.size();y++)
	{
		for (int x = 0;x < map[y].size();x++)
		{
			string c = ".";
		
			if (!map[y][x])
				c = "A";

			ifs << c;
		}

		ifs << endl;
	}*/
}

void Soar_Link::onEnd(bool isWinner)
{
	// Called when the game ends
	if ( isWinner )
	{
		// Log your win here!
	}
}


void Soar_Link::onFrame()
{
	// Called once every game frame

	// Display the game frame rate as text in the upper left area of the screen
	Broodwar->drawTextScreen(200, 0,  "FPS: %d", Broodwar->getFPS() );
	Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS() );

	// Return if the game is a replay or is paused
	if ( Broodwar->isReplay() || Broodwar->isPaused() )
		return;

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
		return;
}

void Soar_Link::onSendText(std::string text)
{
	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());
	cout << "Soar: " << text << endl;

	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!

}

void Soar_Link::onReceiveText(BWAPI::Player* player, std::string text)
{
	// Parse the received text
	cout << player->getName() << "said '" << text << "'" << endl;
	Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

void Soar_Link::onPlayerLeft(BWAPI::Player* player)
{

}

void Soar_Link::onNukeDetect(BWAPI::Position target)
{

}

void Soar_Link::onUnitDiscover(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitEvade(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitShow(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitHide(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitCreate(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitDestroy(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitMorph(BWAPI::Unit* unit)
{

}

void Soar_Link::onUnitRenegade(BWAPI::Unit* unit)
{

}

void Soar_Link::onSaveGame(std::string gameName)
{
	cout << "The game was saved to \"" << gameName << "\"." << endl;
	Broodwar->printf("The game was saved to \"%s\".", gameName.c_str());
}

void Soar_Link::onUnitComplete(BWAPI::Unit *unit)
{

}