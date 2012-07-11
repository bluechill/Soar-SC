#include <iostream>
#include <string>
#include <fstream>

namespace BWAPI
{
	class Player;

	class Game
	{
	public:
		Game(std::string path_to_map_output);

		int mapWidth();
		int mapHeight();

		Player* self();
	};

	class Unitset;

	class Player
	{
	public:
		Unitset getUnits();
		
		Unitset units;
	};

	class Unitset
	{
	public:

	};
};