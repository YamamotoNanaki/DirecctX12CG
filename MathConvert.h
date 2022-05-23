#pragma once
#include <DirectXMath.h>
#include "IFMath.h"

namespace IF
{
	DirectX::XMMATRIX MatrixConvert(Matrix m);
	Matrix MatrixConvert(DirectX::XMMATRIX xm);
}