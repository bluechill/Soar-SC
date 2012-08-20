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

void send_base_input_global(sml::smlAgentEventId id, void* link, sml::Agent* pAgent)
{
	reinterpret_cast<Soar_Link*>(link)->send_base_input(pAgent, true);
}

const DWORD MS_VC_EXCEPTION=0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(const char *threadName, DWORD threadId)
{
	if (!IsDebuggerPresent())
		return;

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = threadId;
	info.dwFlags = 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}
