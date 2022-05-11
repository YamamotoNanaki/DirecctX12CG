#pragma once
#include "Particle.h"
#include <vector>

class Fire
{
	//�l�[���X�y�[�X���g�p
	template <class T> using vector = std::vector<T>;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMMATRIX = DirectX::XMMATRIX;
private:
	unsigned int maxFlame;		//�ő�t���[����
	float scale;				//�傫��
	bool moveOffset = 0;		//2�t���[����1�t���[���ōX�V����

public:
	XMFLOAT3 pos;				//�|�W�V����
	IF::Particle particle[500];	//�p�[�e�B�N��

private:
	//�p�[�e�B�N����ǉ�
	void AddParticle();

public:
	//�ݒ菉����
	Fire(XMFLOAT3 pos, float scale = 1.0f, unsigned int maxFlame = 100);
	//�p�[�e�B�N��������
	HRESULT Initialize(ID3D12Device* device, ID3D12Resource* texBuff, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle);
	void Update(XMMATRIX matView, XMMATRIX matProjection, XMMATRIX matBillBoard = DirectX::XMMatrixIdentity());
	void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport);
};