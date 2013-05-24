#define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include <BWAPI.h>

#include "Soar_SC.h"
#include "BWAPI_Link.h"

#include <memory>

std::unique_ptr<Soar_SC> global_soar_sc_instance;

extern "C" __declspec(dllexport) void gameInit(BWAPI::Game* game) { BWAPI::BroodwarPtr = game; }
BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
			break;
	case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) BWAPI::AIModule* newAIModule()
{
	global_soar_sc_instance = std::unique_ptr<Soar_SC>(new Soar_SC);

	return global_soar_sc_instance->get_bwapi_link();
}
