#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <string>
#include "ModelVI.h"
#include "ConstStruct.h"

#pragma comment(lib,"d3d12.lib") 

namespace IF
{
	class Model
	{
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		template<class T> using vector = std::vector<T>;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMMATRIX = DirectX::XMMATRIX;
	private:
		//定数バッファ
		ComPtr<ID3D12Resource> constBuffTransform1;
		ConstBufferMaterial* constMapMaterial;

	public:
		struct Material
		{
			std::string name;
			XMFLOAT3 ambient;
			XMFLOAT3 diffuse;
			XMFLOAT3 specular;
			float alpha;
			std::string textureFilename;
			Material()
			{
				ambient = { 0.3f,0.3f,0.3f };
				diffuse = { 0.0f,0.0f,0.0f };
				specular = { 0.0f,0.0f,0.0f };
				alpha = 1.0f;
			}
		};

	public:
		VI* vi;
		Material material;
		HRESULT LoadModel(ID3D12Device* device, std::string name);
		HRESULT VIInitialize(ID3D12Device* device);
		void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport, ID3D12Resource* address);
		~Model();
	};
}