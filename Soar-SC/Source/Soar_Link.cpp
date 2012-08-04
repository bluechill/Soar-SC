#include "Soar_Link.h"

using namespace BWAPI;
using namespace sml;
using namespace std;

Soar_Link::Soar_Link()
	: cout_redirect("bwapi-data/logs/stdout.txt"),
	cerr_redirect("bwapi-data/logs/stderr.txt"),
	test_input_file("bwapi-data/logs/test_input.txt"),
	event_queue(NULL, this)
{
	if (!cout_redirect || !cerr_redirect)
		Broodwar->printf("Unable to redirect output!");

	cout_orig_buffer = cout.rdbuf();
	cout.rdbuf(cout_redirect.rdbuf());

	cerr_orig_buffer = cerr.rdbuf();
	cerr.rdbuf(cerr_redirect.rdbuf());

	kernel = Kernel::CreateKernelInNewThread();
	//kernel = Kernel::CreateRemoteConnection(false, "35.0.136.73", 12121);

	mu = SDL_CreateMutex();

	console = new Soar_Console(&event_queue);

	event_queue.set_console(console);

	should_die = false;
	had_interrupt = false;
}

Soar_Link::~Soar_Link()
{
	should_die = true;

	agent->ExecuteCommandLine("stop");

	SDL_WaitThread(soar_thread, NULL);
	SDL_DestroyMutex(mu);

	kernel->DestroyAgent(agent);
	kernel->Shutdown();

	cout.rdbuf(cout_orig_buffer);
	cerr.rdbuf(cerr_orig_buffer);

	delete console;
}
