#include "Soar_Link.h"

using namespace BWAPI;
using namespace std;
using namespace sml;

//Soar Handlers
void output_global_handler(smlRunEventId id, void* d, Agent *a, smlPhase phase)
{
	reinterpret_cast<Soar_Link*>(d)->output_handler(id, d, a, phase);
}

void printcb(smlPrintEventId id, void *d, Agent *a, char const *m)
{
	reinterpret_cast<Soar_Link*>(d)->print_soar(id, d, a, m);
}

//Thread globals
int thread_runner_soar(void* link)
{
	return reinterpret_cast<Soar_Link*>(link)->soar_agent_thread();
}

int thread_runner_console(void *link)
{
	reinterpret_cast<Soar_Link*>(link)->console_function();

	return 0;
}

//Misc globals
string strip(string s, string lc, string rc)
{
	size_t b, e;
	b = s.find_first_not_of(lc);
	e = s.find_last_not_of(rc);
	return s.substr(b, e - b + 1);
}