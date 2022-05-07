#pragma once
#include <stdlib.h>
#include <time.h>

class Rand
{
public:
	static void Initialize()
	{
		srand(time(nullptr));
	}
	static int GetRand()
	{
		return rand();
	}
	static int GetRand(int min, int max)
	{
		return min + rand() % (max - min);
	}
};