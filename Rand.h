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
	static int GetRand(int min = 0, int max = 100)
	{
		return min + rand() % (max - min);
	}
	static float GetRandf(float min = 0, float max = 100, int digit = 3)
	{
		if (digit < 0)return 0;

		int a = 1;

		for (int i = 0; i < digit; i++)
		{
			a *= 10;
		}

		return min + rand() % (a * (int)(max - min)) / a;
	}
};