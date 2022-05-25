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
		//ïœêîêÈåæ
		//-----------------------

		float angleY = 0.0f;
		float angleX = 0.0f;
		int sukai = 0;
		int efect = 0;
		int sky = 0;
		int groundTex = 0;
		Model sphereM;
		Object sphereO;
		DirectX::XMFLOAT3 slDir{ 0,-1,0 };
		DirectX::XMFLOAT3 slPos{ 0,5,0 };
		DirectX::XMFLOAT3 slColor{ 1,1,1 };
		DirectX::XMFLOAT3 slAtten{ 0,0,0 };
		DirectX::XMFLOAT2 slAngle{ 20,30 };
		DirectX::XMFLOAT3 csDir{ 0,-1,0 };
		DirectX::XMFLOAT3 csAtten{ 0.5,0.6,0 };
		DirectX::XMFLOAT2 csAngle{ 0,5 };

		//-----------------------

		//ínñ Ç∆ãÛ
		Model domeM;
		Model groundM;
		Object domeObj;
		Object groundObj;
		//ÉJÉÅÉâ
		View matView;
		Projection* matPro;
		//åıåπ
		LightManager* light = nullptr;

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