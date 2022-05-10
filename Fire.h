#pragma once
#include "Particle.h"
#include <vector>

class Fire
{
	template <class T> using vector = std::vector<T>;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMMATRIX = DirectX::XMMATRIX;
private:
	unsigned int maxFlame;
	float scale;
	bool moveOffset = 0;

public:
	XMFLOAT3 pos;
	IF::Particle particle[500];

public:
	Fire(XMFLOAT3 pos, float scale = 1.0f, unsigned int maxFlame = 100);
	HRESULT Initialize(ID3D12Device* device, ID3D12Resource* texBuff, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle);
	void AddParticle();
	void Update(XMMATRIX matView, XMMATRIX matProjection, XMMATRIX matBillBoard = DirectX::XMMatrixIdentity());
	void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport);
};