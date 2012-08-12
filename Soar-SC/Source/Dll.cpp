#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h> //For windows related stuff
#include <stdio.h> //For standard IO
#include <tchar.h> //For Unicode character

#include <BWAPI.h> //For BWAPI, the starcraft link

#include "Soar_Link.h" //For the AI class
namespace BWAPI { Game* Broodwar; } //Global variable for the BWAPI link

BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved ) //The equivilent of the int main() in a standard program.  Called when the dll is injected
{
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	switch (ul_reason_for_call) //Check for the reason of the function call
	{
	case DLL_PROCESS_ATTACH: //When we are attached to the process
		BWAPI::BWAPI_init(); //Initialize BWAPI
		break; //Then break
	case DLL_PROCESS_DETACH: //Do nothing when we detach, nothing to do.
		break;
	}
	return TRUE; //Return that we were successful
}

extern "C" __declspec(dllexport) BWAPI::AIModule* newAIModule(BWAPI::Game* game) //Called when BWAPI is initialized
{
	BWAPI::Broodwar = game; //Set the global variable
	return new Soar_Link(); //Create an instance of the AI class
}
