#pragma once
#include <DirectXMath.h>


namespace IF
{
	struct ConstBufferDataMaterial
	{
		DirectX::XMFLOAT4 color;	//êF(RGBA)
	};

	struct ConstBufferDataTransform
	{
		DirectX::XMMATRIX mat;
	};
}