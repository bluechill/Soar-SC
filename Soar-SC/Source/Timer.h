#ifndef TIMER_H
#define TIMER_H 1

#include <Windows.h>

class Timer
{
public:
	Timer();
	
	void StartTimer();
	double GetTime();
	double GetTimeMiliseconds();

private:
	double PCFreq;
	__int64 CounterStart;
};

#endif
