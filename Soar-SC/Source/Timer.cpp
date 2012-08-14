#include <iostream>

#include <Windows.h>

#include "Timer.h"

Timer::Timer()
{
	PCFreq = 0.0;
	CounterStart = 0;
}

void Timer::StartTimer()
{
	LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
        std::cerr << "QueryPerformanceFrequency failed!" << std::endl;

    PCFreq = double(li.QuadPart)/1000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

double Timer::GetTime()
{
	LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart)/PCFreq/1000.0;
}

double Timer::GetTimeMiliseconds()
{
	LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart)/PCFreq;
}
