#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

#pragma comment(lib,"d3d12.lib") 

#pragma region �萔�o�b�t�@�\����

struct ConstBufferDataTransform
{
	XMMATRIX mat;
};
#pragma endregion �萔�o�b�t�@�\����

class Object3d
{
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
	Object3d* parent = nullptr;

	void Initialize(HRESULT& result, ID3D12Device* device);
	void Update(XMMATRIX& matView, XMMATRIX& matProjection);
	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT indices);
};