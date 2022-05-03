#include "Object.h"
#include "View.h"

using namespace DirectX;
using namespace IF;

HRESULT Object::Initialize(ID3D12Device* device)
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

	Vertex vertices[] = {
		// x   y   z        u    v
		//前
		{{-5, -5, -5},{0,0,1},{0.0f, 1.0f}},	//左下
		{{-5, +5, -5},{0,0,1},{0.0f, 0.0f}},	//左上
		{{+5, -5, -5},{0,0,1},{1.0f, 1.0f}},	//右下
		{{+5, +5, -5},{0,0,1},{1.0f, 0.0f}},	//右上
		////後			
		//{{+5, -5, +5},{},{1.0f, 1.0f}},	//右下
		//{{+5, +5, +5},{},{1.0f, 0.0f}},	//右上
		//{{-5, -5, +5},{},{0.0f, 1.0f}},	//左下
		//{{-5, +5, +5},{},{0.0f, 0.0f}},	//左上
		////左			
		//{{-5, -5, -5},{},{0.0f, 1.0f}},	//左下
		//{{-5, -5, +5},{},{0.0f, 0.0f}},	//左上
		//{{-5, +5, -5},{},{1.0f, 1.0f}},	//右下
		//{{-5, +5, +5},{},{1.0f, 0.0f}},	//右上
		////右			
		//{{+5, +5, -5},{},{1.0f, 1.0f}},	//右下
		//{{+5, +5, +5},{},{1.0f, 0.0f}},	//右上
		//{{+5, -5, -5},{},{0.0f, 1.0f}},	//左下
		//{{+5, -5, +5},{},{0.0f, 0.0f}},	//左上
		////下			
		//{{-5, +5, +5},{},{1.0f, 1.0f}},	//右下
		//{{+5, +5, +5},{},{1.0f, 0.0f}},	//右上
		//{{-5, +5, -5},{},{0.0f, 1.0f}},	//左下
		//{{+5, +5, -5},{},{0.0f, 0.0f}},	//左上
		////上			
		//{{-5, -5, -5},{},{0.0f, 1.0f}},	//左下
		//{{+5, -5, -5},{},{0.0f, 0.0f}},	//左上
		//{{-5, -5, +5},{},{1.0f, 1.0f}},	//右下
		//{{+5, -5, +5},{},{1.0f, 0.0f}},	//右上
	};

	//インデックスデータ
	unsigned short indices[] = {
		//
		0,1,2,
		2,1,3,
		//
		//4,5,6,
		//6,5,7,
		////
		//8,9,10,
		//10,9,11,
		////
		//12,13,14,
		//14,13,15,
		////
		//16,17,18,
		//18,17,19,
		////
		//20,21,22,
		//22,21,23
	};

	vi = new VI(vertices, _countof(vertices), indices, _countof(indices));

	return result;
}

HRESULT Object::VIInitialize(ID3D12Device* device, ID3D12Resource* texBuff, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle)
{
	HRESULT result = vi->Initialize(device, texBuff, srvHandle);
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
	for (int i = 0; i < viewport.size(); i++)
	{
		commandList->RSSetViewports(1, &viewport[i]);
		//頂点バッファの設定
		commandList->IASetVertexBuffers(0, 1, &vi->vbView);
		//インデックスバッファの設定
		commandList->IASetIndexBuffer(&vi->ibView);
		//定数バッファビューの設定
		commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());
		//描画コマンド
		commandList->DrawIndexedInstanced(vi->indices.size(), 1, 0, 0, 0);
	}
}

Object::~Object()
{
	delete vi;
}
