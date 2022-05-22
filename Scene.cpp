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

	tex->setDevice(device);
	tex->Initialize();
	domeM.LoadModel(device, "skydome");
	groundM.LoadModel(device, "ground");

	domeObj.Initialize(device);
	domeObj.SetModel(&domeM);
	groundObj.Initialize(device);
	groundObj.SetModel(&groundM);
	//fire = new Fire({ 0,0,0 });

	domeM.VIInitialize(device);
	groundM.VIInitialize(device);

	sukai = tex->LoadTexture("Resources/texture.png");
	efect = tex->LoadTexture("Resources/particle.png");

	//result = fire->Initialize(device);
	result = graph.Initialize(device, tex->descRangeSRV);
	//result = pgraph.ParticleInitialize(device, tex->descRangeSRV);

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

	/*if (Key::getInstance().Judge(KEY::Arrow, KEY::OR))
	{
		if (Key::getInstance().Down(KEY::RIGHT))	fire->pos.x += 1.0f;
		if (Key::getInstance().Down(KEY::LEFT))		fire->pos.x -= 1.0f;
		if (Key::getInstance().Down(KEY::UP))		fire->pos.y += 1.0f;
		if (Key::getInstance().Down(KEY::DOWN))		fire->pos.y -= 1.0f;
	}*/

	domeObj.Update(matView.Get(), matPro->Get());
	groundObj.Update(matView.Get(), matPro->Get());
	//fire->Update(matView.Get(), matPro->Get(), matView.matBillBoard);
}

void IF::Scene::Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport)
{
	graph.DrawBlendMode(commandList);
	domeObj.DrawBefore(commandList, graph.rootsignature.Get(), cb.GetGPUAddress());
	domeObj.Draw(commandList, viewport);

	groundObj.DrawBefore(commandList, graph.rootsignature.Get(), cb.GetGPUAddress());
	groundObj.Draw(commandList, viewport);

	//pgraph.DrawBlendMode(commandList, Blend::ADD);
	//tex->setTexture(commandList, efect);
	//fire->particle[0].DrawBefore(commandList, pgraph.rootsignature.Get(), tex->srvHeap.Get(), cb.GetGPUAddress(), D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	//fire->Draw(commandList, viewport);
}
