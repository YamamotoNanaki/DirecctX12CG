#pragma once
#include "ConstBuff.h"
#include "Object.h"
#include "View.h"
#include "Projection.h"
#include "Texture.h"
#include "Graphic.h"
#include "Fire.h"

namespace IF
{
	class Scene
	{
		template <class T>using vector = std::vector<T>;
	private:
		Object object3ds[1];
		//Fire* fire;
		View matView;
		Projection* matPro;
	public:
		ConstBuff cb;
		Texture tex;
		Graphic graph;

	public:
		Scene(float winWidth, float winHeight, HRESULT result, ID3D12Device* device);
		~Scene();

	private:
		//�ϐ��錾
		//-----------------------

		float angleY = 0.0f;
		float angleX = 0.0f;
		//-----------------------

	public:
		void Update(ID3D12Device* device);
		void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport);
	};
}