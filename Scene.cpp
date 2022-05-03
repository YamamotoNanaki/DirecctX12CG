#include "Scene.h"
#include <DirectXMath.h>

using namespace DirectX;

IF::Scene::Scene(float winWidth, float winHeight, HRESULT result, ID3D12Device* device)
{
	matPro = new Projection(45.0f, winWidth, winHeight);

	result = cb.Initialize(device);

	for (int i = 0; i < _countof(object3ds); i++)
	{
		result = object3ds[i].Initialize(device);
		if (i > 0)
		{
			object3ds[i].parent = &object3ds[i - 1];
			object3ds[i].scale = { 0.9f,0.9f,0.9f };
			object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
			object3ds[i].position = { 0.0f,0.0f,-8.0f };
		}
	}

	tex.LoadTexture(L"Resources/texture.png", device);
	for (int i = 0; i < _countof(object3ds); i++)
	{
		object3ds[i].VIInitialize(device, tex.texbuff.Get(), tex.srvHandle);
	}
	result = graph.Initialize(device, tex.descRangeSRV);
}

IF::Scene::~Scene()
{
	delete matPro;
}

void IF::Scene::Update(Key* key)
{
	key->Update();


	if (key->Judge(key->WASD, key->OR))
	{
		if (key->Down(key->D))angleY += XMConvertToRadians(1.0f);
		if (key->Down(key->A))angleY -= XMConvertToRadians(1.0f);
		if (key->Down(key->S))angleX += XMConvertToRadians(1.0f);
		if (key->Down(key->W))angleX -= XMConvertToRadians(1.0f);

		matView.eye.x = -100 * sinf(angleY);
		matView.eye.y = -100 * sinf(angleX);
		matView.eye.z = -100 * cosf(angleY + angleX);

		matView.Update();
	}

	if (key->Judge(key->Arrow, key->OR))
	{
		for (int i = 0; i < 4; i++)
		{
			//‰E
			if (key->Down(key->RIGHT))
			{
				object3ds[i].position.x += 1.0f;
			}
			//¶
			if (key->Down(key->LEFT))
			{
				object3ds[i].position.x -= 1.0f;
			}
			//ã
			if (key->Down(key->UP))
			{
				object3ds[i].position.y += 1.0f;
			}
			//‰º
			if (key->Down(key->DOWN))
			{
				object3ds[i].position.y -= 1.0f;
			}
		}
	}

	for (int i = 0; i < _countof(object3ds); i++)
	{
		object3ds[i].Update(matView.Get(), matPro->Get());
	}
}

void IF::Scene::Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport)
{
	graph.DrawBlendMode(commandList);
	for (int i = 0; i < _countof(object3ds); i++)
	{
		object3ds[i].Draw(commandList, viewport);
	}
}
