#include "Soar_Link.h"

#include <set>
#include <vector>

using namespace BWAPI;
using namespace sml;
using namespace std;

Soar_Link::Soar_Link()
: cout_redirect("bwapi-data/logs/stdout.txt")
{
	if (!cout_redirect)
		Broodwar->printf("Unable to redirect output!");

	cout_orig_buffer = cout.rdbuf();
	cout.rdbuf(cout_redirect.rdbuf());

	////kernel = Kernel::CreateKernelInNewThread();
	kernel = Kernel::CreateRemoteConnection(false, "35.0.136.73", 12121);
}

Soar_Link::~Soar_Link()
{
	cout.rdbuf(cout_orig_buffer);
	kernel->DestroyAgent(agent);
	kernel->Shutdown();
}

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

	cout << "Soar-SC is running." << endl;
	Broodwar->printf("Soar-SC is running.");

	const char* result = agent->ExecuteCommandLine("print s1");
	cout << "Soar: " << result << endl;
	Broodwar->printf("Soar: %s", result);
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
