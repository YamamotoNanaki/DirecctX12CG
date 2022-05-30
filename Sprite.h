#pragma once
#include "SpriteVI.h"
#include "IFMath.h"
#include <vector>
#include <wrl.h>

namespace IF
{
	struct ConstBufferColor
	{
		Float4 color;	//色(RGBA)
	};

	struct ConstBufferMatrix
	{
		Matrix mat;
	};

	class Sprite
	{
		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	private:
		static SV* vi;
		static ID3D12GraphicsCommandList* commandList;
		static ID3D12Device* device;

	public:
		//定数バッファ
		ComPtr<ID3D12Resource> constBuffTransform;
		//定数バッファマップ
		ConstBufferMatrix* constMapTransform;
		//アフィン変換情報
		Float3 scale = { 1,1,1 };
		Float3 rotation = { 0,0,0 };
		Float3 position = { 0,0,0 };
		//ワールド変換行列
		Matrix matWorld;

	public:
		static void SetDeviceCommand(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
		void Initialize();
		void DrawBefore(ID3D12RootSignature* root, D3D12_GPU_VIRTUAL_ADDRESS GPUAddress, D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		void Update();
		void Draw(std::vector<D3D12_VIEWPORT> viewport, unsigned int texNum);
	};
}