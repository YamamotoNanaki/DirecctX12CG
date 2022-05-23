#pragma once
#include "Particle.h"
#include <vector>
#include "IFMath.h"

class Fire
{
	//ネームスペースを使用
	template <class T> using vector = std::vector<T>;
private:
	unsigned int maxFlame;		//最大フレーム数
	float scale;				//大きさ
	bool moveOffset = 0;		//2フレーム中1フレームで更新する

public:
	IF::Float3 pos;				//ポジション
	IF::Particle particle[500];	//パーティクル

private:
	//パーティクルを追加
	void AddParticle();

public:
	//設定初期化
	Fire(IF::Float3 pos, float scale = 1.0f, unsigned int maxFlame = 100);
	//パーティクル初期化
	HRESULT Initialize(ID3D12Device* device);
	void Update(IF::Matrix matView, IF::Matrix matProjection, IF::Matrix matBillBoard = IF::MatrixIdentity());
	void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport);
};