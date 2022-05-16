#include "Scene.h"
#include "Key.h"
#include <DirectXMath.h>
#include "Rand.h"

using namespace DirectX;

IF::Scene::Scene(float winWidth, float winHeight, HRESULT result, ID3D12Device* device)
{
	matPro = new Projection(45.0f, winWidth, winHeight);

	Rand::Initialize();

	result = cb.Initialize(device);

	for (int i = 0; i < _countof(object3ds); i++)
	{
		result = object3ds[i].LoadModelInitialize(device);
		object3ds[i].scale = { 20,20,20 };
		if (i > 0)
		{
			object3ds[i].parent = &object3ds[i - 1];
			object3ds[i].scale = { 0.9f,0.9f,0.9f };
			object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
			object3ds[i].position = { 0.0f,0.0f,-8.0f };
		}
	}
	//fire = new Fire({ 0,0,0 });

	tex.LoadTexture(L"Resources/texture.png", device);

	//result = fire->Initialize(device, tex.texbuff.Get(), tex.srvHandle);
	for (int i = 0; i < _countof(object3ds); i++)
	{
		object3ds[i].VIInitialize(device, tex.texbuff.Get(), tex.srvHandle);
	}
	//result = graph.Initialize(device, tex.descRangeSRV);
	result = Ograph.Initialize(device, tex.descRangeSRV);

	matView.eye = { 0,0,-5.0f };
}

IF::Scene::~Scene()
{
	delete matPro;
}

void IF::Scene::Update(ID3D12Device* device)
{
	Key::getInstance().Update();

	if (Key::getInstance().Judge(KEY::WASD, KEY::OR))
	{
		if (Key::getInstance().Down(KEY::D))angleY += XMConvertToRadians(1.0f);
		if (Key::getInstance().Down(KEY::A))angleY -= XMConvertToRadians(1.0f);
		if (Key::getInstance().Down(KEY::S))angleX += XMConvertToRadians(1.0f);
		if (Key::getInstance().Down(KEY::W))angleX -= XMConvertToRadians(1.0f);

		matView.eye.x = -100 * sinf(angleY);
		matView.eye.y = -100 * sinf(angleX);
		matView.eye.z = -100 * cosf(angleY + angleX);

		matView.Update();
	}

	if (Key::getInstance().Judge(KEY::Arrow, KEY::OR))
	{
		for (int i = 0; i < _countof(object3ds); i++)
		{
			if (Key::getInstance().Down(KEY::RIGHT))	object3ds[i].position.x += 1.0f;
			if (Key::getInstance().Down(KEY::LEFT))		object3ds[i].position.x -= 1.0f;
			if (Key::getInstance().Down(KEY::UP))		object3ds[i].position.y += 1.0f;
			if (Key::getInstance().Down(KEY::DOWN))		object3ds[i].position.y -= 1.0f;
		}
	}

	/*if (Key::getInstance().Judge(KEY::Arrow, KEY::OR))
	{
		if (Key::getInstance().Down(KEY::RIGHT))	fire->pos.x += 1.0f;
		if (Key::getInstance().Down(KEY::LEFT))		fire->pos.x -= 1.0f;
		if (Key::getInstance().Down(KEY::UP))		fire->pos.y += 1.0f;
		if (Key::getInstance().Down(KEY::DOWN))		fire->pos.y -= 1.0f;
	}*/

	for (int i = 0; i < _countof(object3ds); i++)
	{
		object3ds[i].Update(matView.Get(), matPro->Get(), object3ds[0].NOON);
	}
	//fire->Update(matView.Get(), matPro->Get(), matView.matBillBoard);
}

void IF::Scene::Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport)
{
	object3ds->DrawBefore(commandList, Ograph.rootsignature.Get(), tex.srvHeap, cb.GetGPUAddress());
	Ograph.DrawBlendMode(commandList);
	for (int i = 0; i < _countof(object3ds); i++)
	{
		object3ds[i].Draw(commandList, viewport);
	}
	/*cb.SetBright(175, 25, 5);
	graph.DrawBlendMode(commandList, graph.ADD);
	fire->particle[0].DrawBefore(commandList, graph.rootsignature.Get(), tex.srvHeap, cb.GetGPUAddress(), D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	fire->Draw(commandList, viewport);*/
}
