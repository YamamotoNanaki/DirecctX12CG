#pragma once
#include <DirectXMath.h>


namespace IF
{
	struct ConstBufferDataColor
	{
		DirectX::XMFLOAT4 color;	//êF(RGBA)
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
	struct ConstBufferMaterial
	{
		DirectX::XMFLOAT3 ambient;
		float pad1;
		DirectX::XMFLOAT3 diffuse;
		float pad2;
		DirectX::XMFLOAT3 specular;
		float alpha;
	};
}