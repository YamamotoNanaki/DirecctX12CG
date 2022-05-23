#include "Projection.h"
#include "MathConvert.h"

using namespace IF;
using namespace DirectX;

Projection::Projection(float radian, float winWidth, float winHeight, float nearZ, float farZ)
{
	Inisialize(radian, winWidth, winHeight, nearZ, farZ);
}

void Projection::Inisialize(float radian, float winWidth, float winHeight, float nearZ, float farZ)
{
	XMMATRIX m = XMMatrixPerspectiveFovLH(ConvertToRadians(radian), (float)winWidth / winHeight, nearZ, farZ);
	matProjection = MatrixConvert(m);
}

Matrix Projection::Get() const
{
	return matProjection;
}
