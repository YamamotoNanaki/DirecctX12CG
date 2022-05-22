#include "Object.h"
#include "View.h"
#include <fstream>
#include <sstream>
#include <string>


using namespace DirectX;
using namespace IF;
using namespace std;
using namespace IF::BillBoard;

void IF::Object::DrawBefore(ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* root, ID3D12DescriptorHeap* srvHeap, D3D12_GPU_VIRTUAL_ADDRESS GPUAddress, D3D_PRIMITIVE_TOPOLOGY topology)
{
	commandList->SetGraphicsRootSignature(root);
	commandList->IASetPrimitiveTopology(topology);
	commandList->SetGraphicsRootConstantBufferView(0, GPUAddress);
}

void IF::Object::SetModel(Model* model)
{
	this->model = model;
}

HRESULT IF::Object::Initialize(ID3D12Device* device)
{
	HRESULT result;
	//定数バッファのヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//定数バッファのリソース設定
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//定数バッファの生成
	result = device->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE, &resdesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuffTransform));
	assert(SUCCEEDED(result));


	//定数バッファのマッピング
	result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);
	assert(SUCCEEDED(result));

	return result;
}

void Object::Update(XMMATRIX matView, XMMATRIX matProjection, BillBoardMode mode)
{
	XMMATRIX matScale, matRot, matTrams;

	//スケール、回転、平行移動
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(rotation.z);
	matRot *= XMMatrixRotationZ(rotation.x);
	matRot *= XMMatrixRotationZ(rotation.y);
	matTrams = XMMatrixTranslation(position.x, position.y, position.z);
	//ワールド行列の合成
	matWorld = XMMatrixIdentity();
	if (mode == BILLBOARD)matWorld *= View::matBillBoard;
	else if (mode == YBOARD)matWorld *= View::matBillBoardY;
	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrams;
	//親オブジェクトがあれば
	if (parent != nullptr)
	{
		matWorld *= parent->matWorld;
	}

	//定数バッファへのデータ転送
	constMapTransform->mat = matWorld * matView * matProjection;
}

void Object::Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport)
{
	if (model == nullptr)
	{
		assert(0 && "モデルがセットされていません");
		return;
	}
	model->Draw(commandList, viewport, constBuffTransform.Get());
}

Object::~Object()
{
	constBuffTransform->Unmap(0, nullptr);
}
