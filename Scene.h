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
		//�ϐ��錾
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

		//�n�ʂƋ�
		Model domeM;
		Model groundM;
		Object domeObj;
		Object groundObj;
		//�J����
		View matView;
		Projection* matPro;

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