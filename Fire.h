#pragma once
#include "Particle.h"
#include <vector>
#include "IFMath.h"

class Fire
{
	//�l�[���X�y�[�X���g�p
	template <class T> using vector = std::vector<T>;
private:
	unsigned int maxFlame;		//�ő�t���[����
	float scale;				//�傫��
	bool moveOffset = 0;		//2�t���[����1�t���[���ōX�V����

public:
	IF::Float3 pos;				//�|�W�V����
	IF::Particle particle[500];	//�p�[�e�B�N��

private:
	//�p�[�e�B�N����ǉ�
	void AddParticle();

public:
	//�ݒ菉����
	Fire(IF::Float3 pos, float scale = 1.0f, unsigned int maxFlame = 100);
	//�p�[�e�B�N��������
	HRESULT Initialize(ID3D12Device* device);
	void Update(IF::Matrix matView, IF::Matrix matProjection, IF::Matrix matBillBoard = IF::MatrixIdentity());
	void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport);
};