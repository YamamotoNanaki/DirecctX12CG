#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include "ParticleVI.h"
#include "ConstStruct.h"
#include "IFMath.h"

#pragma comment(lib,"d3d12.lib") 

namespace IF
{
	class Particle
	{
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		template<class T> using vector = std::vector<T>;
	public:
	enum BillBoardMode
	{
		NOON,
		BILLBOARD,
		YBOARD
	};
	private:
		VI* vi;

	public:
		//定数バッファ
		ComPtr<ID3D12Resource> constBuffTransform;
		//定数バッファマップ
		ConstBufferBillboard* constMapTransform;
		//アフィン変換情報
		Float3 position = { 0,0,0 };
		//ワールド変換行列
		Matrix matWorld;
		float scale = 1;
		unsigned int flame = 0;
		bool flag = 0;
		float targetY;
		float startY;

	public:
		HRESULT Initialize(ID3D12Device* device);
		HRESULT VIInitialize(ID3D12Device* device);
		void Update(Matrix matView, Matrix matProjection, Matrix matBillBoard = MatrixIdentity());
		void DrawBefore(ID3D12GraphicsCommandList* commandList,ID3D12RootSignature* root, ID3D12DescriptorHeap* srvHeap, D3D12_GPU_VIRTUAL_ADDRESS GPUAddress,
			D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport);
		~Particle();
	};
}