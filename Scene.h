#pragma once
#include "ConstBuff.h"
#include "Object.h"
#include "Model.h"
#include "View.h"
#include "Projection.h"
#include "Texture.h"
#include "Graphic.h"
#include "Fire.h"
#include "Light.h"

namespace IF
{
	class Scene
	{
		template <class T>using vector = std::vector<T>;
	private:
		//変数宣言
		//-----------------------

		float angleY = 0.0f;
		float angleX = 0.0f;
		int sukai = 0;
		int efect = 0;
		int sky = 0;
		int groundTex = 0;
		Model sphereM;
		Object sphereO;

		//-----------------------

		//地面と空
		Model domeM;
		Model groundM;
		Object domeObj;
		Object groundObj;
		//カメラ
		View matView;
		Projection* matPro;
		//光源
		Light* light = nullptr;

	public:
		ConstBuff cb;
		Texture* tex = Texture::getInstance();
		Graphic graph;

	public:
		Scene(float winWidth, float winHeight, HRESULT result, ID3D12Device* device);
		~Scene();

	public:
		void Update(ID3D12Device* device);
		void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport);
	};
}