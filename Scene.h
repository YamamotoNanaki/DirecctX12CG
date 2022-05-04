#pragma once
#include "ConstBuff.h"
#include "Object.h"
#include "View.h"
#include "Projection.h"
#include "Texture.h"
#include "ObjGraphic.h"
#include "ParticleGrap.h"
#include "Particle.h"

namespace IF
{
	class Scene
	{
		template <class T>using vector = std::vector<T>;
	private:
		//Object object3ds[1];
		Particle particle[1];
		View matView;
		Projection* matPro;
	public:
		//ConstBuff cb;
		Texture tex;
		//ObjGraphic graph;
		ParticleGrap graph;

	public:
		Scene(float winWidth, float winHeight, HRESULT result, ID3D12Device* device);
		~Scene();

	private:
		//ïœêîêÈåæ
		//-----------------------

		float angleY = 0.0f;
		float angleX = 0.0f;
		//-----------------------

	public:
		void Update();
		void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport);
	};
}