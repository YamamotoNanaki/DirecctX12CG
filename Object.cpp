#include "Object.h"
#include "View.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include "MathConvert.h"
#include <DirectXMath.h>


using namespace IF;
using namespace std;
using namespace IF::BillBoard;
using namespace DirectX;

void IF::Object::DrawBefore(ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* root, D3D12_GPU_VIRTUAL_ADDRESS GPUAddress, D3D_PRIMITIVE_TOPOLOGY topology)
{
	commandList->SetGraphicsRootSignature(root);
	commandList->IASetPrimitiveTopology(topology);
	commandList->SetGraphicsRootConstantBufferView(0, GPUAddress);
}

void IF::Object::SetModel(Model* model)
{
	this->model = model;
}

HRESULT IF::Object::Initialize(ID3D12Device* device, Model* model)
{
	HRESULT result;
	//�萔�o�b�t�@�̃q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//�萔�o�b�t�@�̃��\�[�X�ݒ�
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE, &resdesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuffTransform));
	assert(SUCCEEDED(result));


	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);
	assert(SUCCEEDED(result));

	this->model = model;

	return result;
}

void Object::Update(Matrix matView, Matrix matProjection, BillBoardMode mode)
{
	XMMATRIX matScale, matRot, matTrams, matWorld;

	//�X�P�[���A��]�A���s�ړ�
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(rotation.z);
	matRot *= XMMatrixRotationX(rotation.x);
	matRot *= XMMatrixRotationY(rotation.y);
	matTrams = XMMatrixTranslation(position.x, position.y, position.z);
	//���[���h�s��̍���
	matWorld = XMMatrixIdentity();
	/*if (mode == BILLBOARD)matWorld *= View::matBillBoard;
	else if (mode == YBOARD)matWorld *= View::matBillBoardY;*/
	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrams;
	//�e�I�u�W�F�N�g�������
	if (parent != nullptr)
	{
		//matWorld *= parent->matWorld;
	}


	//�萔�o�b�t�@�ւ̃f�[�^�]��
	constMapTransform->mat = matWorld * MatrixConvert(matView * matProjection);
}

void Object::Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport)
{
	if (model == nullptr)
	{
		assert(0 && "���f�����Z�b�g����Ă��܂���");
		return;
	}
	model->Draw(commandList, viewport, constBuffTransform.Get());
}

Object::~Object()
{
	constBuffTransform->Unmap(0, nullptr);
}
