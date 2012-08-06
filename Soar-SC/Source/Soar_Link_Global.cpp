#include "Soar_Link.h" //Include Soar Link class header

using namespace BWAPI; //Use namespace to allow use of string instead of std::string or Agent* instead of sml::Agent* for example
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

void misc_global_handler(sml::smlRunEventId id, void* d, sml::Agent *a, sml::smlPhase phase)
{
	reinterpret_cast<Soar_Link*>(d)->misc_handler(id, d, a, phase);
}

//Thread globals
int thread_runner_soar(void* link)
{
	return reinterpret_cast<Soar_Link*>(link)->soar_agent_thread();
}
