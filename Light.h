#pragma once
#pragma once
#include <wrl.h>
#include <DirectXMath.h>
#include <d3dx12.h>

namespace IF
{
	class Light
	{
		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMVECTOR = DirectX::XMVECTOR;
		using XMMATRIX = DirectX::XMMATRIX;
	public:
		struct ConstBufferData
		{
			XMVECTOR lightv;
			XMFLOAT3 lightcolor;
		};

	private:
		static ID3D12Device* device;
		ComPtr<ID3D12Resource> constBuff;
		XMVECTOR lightDir = { 1,0,0,0 };
		XMFLOAT3 lightColor = { 1,1,1 };
		bool dirty = false;

	public:
		static void SetDevice(ID3D12Device* device);
		void Initialize();
		void TransferConstBuffer();
		void SetLightDir(const XMVECTOR& lightDir);
		void SetLightColor(const XMFLOAT3& lightColor);
		void Update();
		void Draw(ID3D12GraphicsCommandList*commandList,UINT rootParameterIndex);
		static Light* GetInstance();
	};
}