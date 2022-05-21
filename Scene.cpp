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
		result = object3ds[i].Initialize(device);
		object3ds[i].scale = { 20,20,20 };
		if (i > 0)
		{
			object3ds[i].parent = &object3ds[i - 1];
			object3ds[i].scale = { 0.9f,0.9f,0.9f };
			object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
			object3ds[i].position = { 0.0f,0.0f,-8.0f };
		}
	}
	fire = new Fire({ 0,0,0 });

	for (int i = 0; i < _countof(object3ds); i++)
	{
		object3ds[i].VIInitialize(device);
	}

	tex->setDevice(device);
	tex->Initialize();
	sukai = tex->LoadTexture("Resources/texture.png");
	efect = tex->LoadTexture("Resources/particle.png");

	result = fire->Initialize(device);
	result = graph.Initialize(device, tex->descRangeSRV);
	result = pgraph.ParticleInitialize(device, tex->descRangeSRV);

	matView.eye = { 0,0,-5.0f };
}

IF::Scene::~Scene()
{
	delete matPro;
}

void IF::Scene::Update(ID3D12Device* device)
{
	Key* key = Key::getInstance();
	key->Update();

	if (key->Judge(KEY::WASD, KEY::OR))
	{
		if (key->Down(KEY::D))angleY += XMConvertToRadians(1.0f);
		if (key->Down(KEY::A))angleY -= XMConvertToRadians(1.0f);
		if (key->Down(KEY::S))angleX += XMConvertToRadians(1.0f);
		if (key->Down(KEY::W))angleX -= XMConvertToRadians(1.0f);

		matView.eye.x = -100 * sinf(angleY);
		matView.eye.y = -100 * sinf(angleX);
		matView.eye.z = -100 * cosf(angleY + angleX);

		matView.Update();
	}

	if (key->Judge(KEY::Arrow, KEY::OR))
	{
		for (int i = 0; i < _countof(object3ds); i++)
		{
			if (key->Down(KEY::RIGHT))	object3ds[i].position.x += 1.0f;
			if (key->Down(KEY::LEFT))		object3ds[i].position.x -= 1.0f;
			if (key->Down(KEY::UP))		object3ds[i].position.y += 1.0f;
			if (key->Down(KEY::DOWN))		object3ds[i].position.y -= 1.0f;
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
	fire->Update(matView.Get(), matPro->Get(), matView.matBillBoard);
}

void IF::Scene::Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport)
{
	graph.DrawBlendMode(commandList);
	object3ds->DrawBefore(commandList, graph.rootsignature.Get(), tex->srvHeap.Get(), cb.GetGPUAddress());
	tex->setTexture(commandList, sukai);
	for (int i = 0; i < _countof(object3ds); i++)
	{
		object3ds[i].Draw(commandList, viewport);
	}
	pgraph.DrawBlendMode(commandList,Blend::ADD);
	tex->setTexture(commandList, efect);
	fire->particle[0].DrawBefore(commandList, pgraph.rootsignature.Get(), tex->srvHeap.Get(), cb.GetGPUAddress(), D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	fire->Draw(commandList, viewport);
}
