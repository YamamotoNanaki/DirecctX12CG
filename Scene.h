#pragma once
#include "ConstBuff.h"
#include "Object.h"
#include "Model.h"
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
		Model object3ds[1];
		//Fire* fire;
		View matView;
		Projection* matPro;
	public:
		ConstBuff cb;

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
		void Update(ID3D12Device* device);
		void Update();
		void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport);
	};
}