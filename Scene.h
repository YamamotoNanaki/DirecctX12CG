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
#include "Sprite.h"
#include "IScene.h"
#include "Sound.h"

#ifdef _DEBUG
#include "DebugText.h"

#endif // _DEBUG


namespace IF
{
	class Scene :public IScene
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
		Model sphereM{};
		Object sphereO{};
		DirectX::XMFLOAT3 slDir{ 0.0f,-1.0f,0.0f };
		DirectX::XMFLOAT3 slPos{ 0.0f,5.0f,0.0f };
		DirectX::XMFLOAT3 slColor{ 1.0f,1.0f,1.0f };
		DirectX::XMFLOAT3 slAtten{ 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT2 slAngle{ 20.0f,30.0f };
		DirectX::XMFLOAT3 csDir{ 0.0f,-1.0f,0.0f };
		DirectX::XMFLOAT3 csAtten{ 0.5f,0.6f,0.0f };
		DirectX::XMFLOAT2 csAngle{ 0.0f,0.7f };
		Sprite sprite{};
		unsigned int SGraph = 0;
		soundwave::SoundData testSound{};

		//-----------------------

		//�n�ʂƋ�
		Model domeM{};
		Model groundM{};
		Object domeObj{};
		Object groundObj{};
		//�J����
		View matView;
		Projection* matPro = nullptr;
		//����
		LightManager* light = nullptr;
		//����
		Sound* sound = Sound::Instance();

	public:
		ConstBuff cb;
		Texture* tex = Texture::Instance();
		Graphic graph;

	private:
		int winWidth;
		int winHeight;
		ID3D12Device* device;
		ID3D12GraphicsCommandList* commandList;
		vector<D3D12_VIEWPORT> viewport;

		//�f�o�b�O�p
#ifdef _DEBUG
		DebugText dText;

#endif // _DEBUG


	public:
		Scene(int winWidth, int winHeight, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport);
		~Scene();

		void Initialize();
		void Update();
		void Draw();
	};
}