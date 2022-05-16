#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <string>
#include "ObjVI.h"
#include "ConstStruct.h"

#pragma comment(lib,"d3d12.lib") 

namespace IF
{
	class Object
	{
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		template<class T> using vector = std::vector<T>;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMMATRIX = DirectX::XMMATRIX;
	public:
		enum BillBoardMode
		{
			NOON,
			BILLBOARD,
			YBOARD
		};
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

	private:
		objVI* vi;
		Material material;

	public:
		//�萔�o�b�t�@
		ComPtr<ID3D12Resource> constBuffTransform;
		//�萔�o�b�t�@
		ComPtr<ID3D12Resource> constBuffTransform1;
		//�萔�o�b�t�@�}�b�v
		ConstBufferDataTransform* constMapTransform;
		ConstBufferMaterial* constMapMaterial;
		//�A�t�B���ϊ����
		XMFLOAT3 scale = { 1,1,1 };
		XMFLOAT3 rotation = { 0,0,0 };
		XMFLOAT3 position = { 0,0,0 };
		//���[���h�ϊ��s��
		XMMATRIX matWorld;
		//�e�I�u�W�F�N�g�ւ̃|�C���^
		Object* parent = nullptr;

	public:
		HRESULT Initialize(ID3D12Device* device);
		HRESULT LoadModelInitialize(ID3D12Device* device);
		HRESULT VIInitialize(ID3D12Device* device, ID3D12Resource* texBuff, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle);
		void DrawBefore(ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* root, ID3D12DescriptorHeap* srvHeap, D3D12_GPU_VIRTUAL_ADDRESS GPUAddress,
			D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		void Update(XMMATRIX matView, XMMATRIX matProjection, BillBoardMode mode);
		void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport);
		~Object();
	};
}