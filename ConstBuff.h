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
		ConstBufferDataColor* constMapMaterial = nullptr;
	public:
		HRESULT Initialize(ID3D12Device* device);
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress();

	private:
		short R = 0, G = 0, B = 0, A = 0;
		float r = 0, g = 0, b = 0, a = 0;

	public:
		HRESULT SetBright(int red, int green, int blue);
		HRESULT SetAlpha(int alpha);
		HRESULT SetColor(int red, int green, int blue, int alpha);
	};
}