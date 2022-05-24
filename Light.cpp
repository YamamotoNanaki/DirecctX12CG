#include "Light.h"

using namespace DirectX;
using namespace IF;

ID3D12Device* IF::Light::device = nullptr;

void IF::Light::SetDevice(ID3D12Device* device)
{
	assert(!Light::device);
	assert(device);
	Light::device = device;
}

void IF::Light::Initialize()
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES a = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC b = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

	result = device->CreateCommittedResource(&a, D3D12_HEAP_FLAG_NONE, &b, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&constBuff));

	assert(SUCCEEDED(result));

	TransferConstBuffer();
}

void IF::Light::TransferConstBuffer()
{
	HRESULT result;
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
	constMap->lightv = -lightDir;
	constMap->lightcolor = lightColor;
	constBuff->Unmap(0, nullptr);
}

void IF::Light::SetLightDir(const XMVECTOR& lightdir)
{
	this->lightDir = XMVector3Normalize(lightDir);
	dirty = true;
}

void IF::Light::SetLightColor(const XMFLOAT3& lightColor)
{
	this->lightColor = lightColor;
	dirty = true;
}

void IF::Light::Update()
{
	if (dirty)
	{
		TransferConstBuffer();
		dirty = false;
	}
}

void IF::Light::Draw(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuff->GetGPUVirtualAddress());
}

Light* IF::Light::GetInstance()
{
	Light* instance = new Light;
	return instance;
}
