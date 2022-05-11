#include "Particle.h"
#include "View.h"

using namespace DirectX;
using namespace IF;

HRESULT Particle::Initialize(ID3D12Device* device)
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
	constBuffTransform->Unmap(0, nullptr);

	VertexPos vertices[] = {
		// x   y   z        u    v
		//前
		{{0, 0, 0}},	//左下
	};


	vi = new PVI(vertices, _countof(vertices));

	return result;
}

HRESULT Particle::VIInitialize(ID3D12Device* device, ID3D12Resource* texBuff, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle)
{
	HRESULT result = vi->Initialize(device, texBuff, srvHandle);
	return result;
}

void Particle::Update(XMMATRIX matView, XMMATRIX matProjection, XMMATRIX matBillBoard)
{
	XMMATRIX matScale, matTrams;

	//スケール、回転、平行移動
	matScale = XMMatrixScaling(scale, scale, scale);
	matTrams = XMMatrixTranslation(position.x, position.y, position.z);
	//ワールド行列の合成
	matWorld = XMMatrixIdentity();
	matWorld *= matScale;
	matWorld *= matTrams;

	//定数バッファへのデータ転送
	constMapTransform->mat = matWorld * matView * matProjection;
	constMapTransform->matBillboard = matBillBoard;
}

void IF::Particle::DrawBefore(ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* root, ID3D12DescriptorHeap* srvHeap, D3D12_GPU_VIRTUAL_ADDRESS GPUAddress, D3D_PRIMITIVE_TOPOLOGY topology)
{
	commandList->SetGraphicsRootSignature(root);
	commandList->IASetPrimitiveTopology(topology);
	commandList->SetGraphicsRootConstantBufferView(0, GPUAddress);
	commandList->SetDescriptorHeaps(1, &srvHeap);
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
}

void Particle::Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport)
{
	for (int i = 0; i < viewport.size(); i++)
	{
		commandList->RSSetViewports(1, &viewport[i]);
		//頂点バッファの設定
		commandList->IASetVertexBuffers(0, 1, &vi->vbView);
		//定数バッファビューの設定
		commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());
		//描画コマンド
		commandList->DrawInstanced(vi->vertices.size(), 1, 0, 0);
	}
}

Particle::~Particle()
{
	delete vi;
}
