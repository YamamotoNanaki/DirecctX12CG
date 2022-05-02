#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <WRL.h>
#include "ConstStruct.h"

namespace IF
{
	class ConstBuff
	{
		using XMFLOAT4 = DirectX::XMFLOAT4;
		template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	private:
		ComPtr<ID3D12Resource> constBuffMaterial = nullptr;
		ConstBufferDataMaterial* constMapMaterial = nullptr;
	public:
		HRESULT Initialize(ID3D12Device* device);
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress();
	};
}