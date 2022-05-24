#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <string>
#include "ModelVI.h"
#include "ConstStruct.h"
#include "Model.h"

#pragma comment(lib,"d3d12.lib") 

namespace IF
{
	namespace BillBoard
	{
		enum BillBoardMode
		{
			NOON,
			BILLBOARD,
			YBOARD
		};
	}

	class Object
	{
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		template<class T> using vector = std::vector<T>;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMMATRIX = DirectX::XMMATRIX;
	private:
		Model* model;

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
		HRESULT Initialize(ID3D12Device* device, Model* model);
		void SetModel(Model* model);
		void DrawBefore(ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* root, D3D12_GPU_VIRTUAL_ADDRESS GPUAddress,
			D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		void Update(XMMATRIX matView, XMMATRIX matProjection, BillBoard::BillBoardMode mode = BillBoard::NOON);
		void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport);
		~Object();
	};
}