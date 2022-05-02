#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "VertexIndex.h"

#pragma comment(lib,"d3d12.lib") 

namespace IF
{

#pragma region �萔�o�b�t�@�\����

	struct ConstBufferDataTransform
	{
		DirectX::XMMATRIX mat;
	};
#pragma endregion �萔�o�b�t�@�\����

	class Object
	{
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMMATRIX = DirectX::XMMATRIX;
	private:
		VI* vi;

	public:
		//�萔�o�b�t�@
		ComPtr<ID3D12Resource> constBuffTransform;
		//�萔�o�b�t�@�}�b�v
		ConstBufferDataTransform* constMapTransform;
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
		HRESULT VIInitialize(ID3D12Device* device, ID3D12Resource* texBuff, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle);
		void Update(XMMATRIX matView, XMMATRIX matProjection);
		void Draw(ID3D12GraphicsCommandList* commandList);
		~Object();
	};
}