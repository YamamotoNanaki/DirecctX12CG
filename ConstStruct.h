#pragma once
#include <DirectXMath.h>


namespace IF
{
	struct ConstBufferDataMaterial
	{
		DirectX::XMFLOAT4 color;	//�F(RGBA)
	};

	struct ConstBufferDataTransform
	{
		DirectX::XMMATRIX mat;
	};

	struct ConstBufferBillboard
	{
		DirectX::XMMATRIX mat;
		DirectX::XMMATRIX matBillboard;
	};
}