#pragma once
#include "IFMath.h"

namespace IF
{
	class Projection
	{
	private:
		Matrix matProjection;
		float nearZ, farZ;

	public:
		Projection(float radian, float winWidth, float winHeight, float nearZ = 0.1f, float farZ = 1000.0f);
		void Inisialize(float radian, float winWidth, float winHeight, float nearZ = 0.1f, float farZ = 1000.0f);

		Matrix Get()const;
	};
}