#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>
#include "ModelVI.h"
#include "ConstStruct.h"
#include "IFMath.h"

#pragma comment(lib,"d3d12.lib") 

namespace IF
{
	class Model
	{
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		template<class T> using vector = std::vector<T>;
	private:
		//�萔�o�b�t�@
		ComPtr<ID3D12Resource> constBuffTransform1;
		ConstBufferMaterial* constMapMaterial;

	public:
		struct Material
		{
			std::string name;
			DirectX::XMFLOAT3 ambient;
			DirectX::XMFLOAT3 diffuse;
			DirectX::XMFLOAT3 specular;
			float alpha;
			std::string textureFilename;
			Material()
			{
				ambient = { 0.3f,0.3f,0.3f };
				diffuse = { 0.0f,0.0f,0.0f };
				specular = { 0.0f,0.0f,0.0f };
				alpha = 1.0f;
			}
			bool tex = false;
			unsigned short texNum = 0;
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