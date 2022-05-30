#include "Sprite.h"
#include <cassert>
#include "Texture.h"

using namespace IF;

SV* Sprite::vi = nullptr;
ID3D12GraphicsCommandList* Sprite::commandList = nullptr;
ID3D12Device* Sprite::device = nullptr;


void IF::Sprite::SetDeviceCommand(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	assert(device);
	assert(commandList);

	Sprite::device = device;
	Sprite::commandList = commandList;
}

void IF::Sprite::Initialize()
{
	vi = new SV;

	Vertex2D vertices[]
	{
		{{-5.0f,+5.0f,0.0f},{0.0f,1.0f}},
		{{-5.0f,-5.0f,0.0f},{0.0f,0.0f}},
		{{+5.0f,-5.0f,0.0f},{1.0f,0.0f}}
	};

	vi->SetVerticle(vertices);
	vi->Initialize(Sprite::device);

	HRESULT result;
	//定数バッファのヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//定数バッファのリソース設定
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = (sizeof(ConstBufferMatrix) + 0xff) & ~0xff;
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
}

void IF::Sprite::DrawBefore(ID3D12RootSignature* root, D3D12_GPU_VIRTUAL_ADDRESS GPUAddress, D3D_PRIMITIVE_TOPOLOGY topology)
{
	commandList->SetGraphicsRootSignature(root);
	commandList->IASetPrimitiveTopology(topology);
	commandList->SetGraphicsRootConstantBufferView(0, GPUAddress);
}

void IF::Sprite::Update()
{
	Matrix matScale, matRot, matTrams;

	//スケール、回転、平行移動
	matScale = MatrixScaling(scale.x, scale.y, scale.z);
	matRot = MatrixIdentity();
	matRot *= MatrixRotationZ(rotation.z);
	matRot *= MatrixRotationX(rotation.x);
	matRot *= MatrixRotationY(rotation.y);
	matTrams = MatrixTranslation(position.x, position.y, position.z);
	//ワールド行列の合成
	matWorld = MatrixIdentity();
	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrams;

	//定数バッファへのデータ転送
	constMapTransform->mat = matWorld;
}

void IF::Sprite::Draw(std::vector<D3D12_VIEWPORT> viewport,unsigned int texNum)
{
	for (int i = 0; i < viewport.size(); i++)
	{
		Texture::getInstance()->setTexture(commandList, texNum);
		commandList->RSSetViewports(1, &viewport[i]);
		//頂点バッファの設定
		commandList->IASetVertexBuffers(0, 1, &vi->GetVertexView());
		//定数バッファビューの設定
		commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());
		//描画コマンド
		commandList->DrawInstanced(vi->GetSize(), 1, 0, 0);
	}
}
