#include "Sprite.h"
#include <cassert>

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

void IF::Sprite::Update(Matrix matView, Matrix matProjection, Float3 comeraPos)
{

}

void IF::Sprite::Draw(std::vector<D3D12_VIEWPORT> viewport)
{

}
