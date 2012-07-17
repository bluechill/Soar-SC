#include "Soar_Link.h"

#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <conio.h>

using namespace BWAPI;
using namespace std;
using namespace sml;

const std::string Soar_Link::unit_box_verts = "0 0 0 0 0 1 0 1 0 0 1 1 1 0 0 1 0 1 1 1 0 1 1 1";

float Soar_Link::flip_one_d_point(const float &point, const bool &x_axis)
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

static const WORD MAX_CONSOLE_LINES = 500;

void Soar_Link::RedirectIO()
{
	using namespace std;

	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;

	// allocate a console for this app
	AllocConsole();

	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),coninfo.dwSize);

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stdout = *fp;

	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console

	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stderr = *fp;

	setvbuf( stderr, NULL, _IONBF, 0 );

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	ios::sync_with_stdio();
}


void Soar_Link::print_soar(smlPrintEventId id, void *d, Agent *a, char const *m)
{
	SDL_mutexP(mu);
	console_buffer.push_back(string(strip(m, "\n", "\n\t ")));
	SDL_mutexV(mu);
}

void Soar_Link::console_function()
{
	RedirectIO();

	agent->RegisterForPrintEvent(smlEVENT_PRINT, printcb, this);

	while (!should_die)
	{
		if (_kbhit())
		{
			string line;
			SDL_mutexP(mu);
			getline(cin, line);

			line.erase(line.begin(), line.begin()+2);

			const char* output = NULL;
			if ((output = agent->ExecuteCommandLine(line.c_str())))
				console_buffer.push_back(string(output));

			SDL_mutexV(mu);
		}

		SDL_mutexP(mu);
		if (console_buffer.size() > 0)
		{
			cout << console_buffer[0] << endl;

			console_buffer.erase(console_buffer.begin());
		}
		SDL_mutexV(mu);
	}
}