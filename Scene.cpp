#include "Scene.h"
#include "Input.h"
#include <DirectXMath.h>
#include "Rand.h"
#include "Light.h"

using namespace DirectX;
using namespace std;

IF::Scene::Scene(float winWidth, float winHeight, HRESULT result, ID3D12Device* device)
{
	light = Light::GetInstance();
	light->Initialize();
	light->SetLightColor({ 1, 1, 1 });
	Object::SetLight(light);
	//定数バッファの初期化
	result = cb.Initialize(device);
	//画像関連初期化
	tex->setDevice(device);
	tex->Initialize();
	result = graph.Initialize(device, tex->descRangeSRV);
	//モデルの読み込みとオブジェクトとの紐付け(空と地面)
	domeM.LoadModel(device, "skydome");
	groundM.LoadModel(device, "ground");
	domeObj.Initialize(device, &domeM);
	groundObj.Initialize(device, &groundM);
	groundObj.position = { 0,-2,0 };
	//カメラ関連初期化
	matPro = new Projection(45.0f, winWidth, winHeight);
	matView.eye = { 0,0,-5.0f };

	//そのほかの初期化
	Rand::Initialize();

	sphereM.LoadModel(device, "sphere", true);
	sphereO.Initialize(device, &sphereM);

	sphereO.position = { 0,0,0 };
	matView.Update();
}

IF::Scene::~Scene()
{
	delete matPro;
	delete light;
}

void IF::Scene::Update(ID3D12Device* device)
{
	Input* input = Input::getInstance();
	input->Update();

	//光源
	static XMVECTOR lightDir = { 0,1,5,0 };

	if (input->KDown(KEY::W))lightDir.m128_f32[1] += 1.0f;
	if (input->KDown(KEY::S))lightDir.m128_f32[1] -= 1.0f;
	if (input->KDown(KEY::D))lightDir.m128_f32[0] += 1.0f;
	if (input->KDown(KEY::A))lightDir.m128_f32[0] -= 1.0f;

	light->SetLightDir(lightDir);

	light->Update();

	//カメラ
	if (input->KDown(KEY::UP))
	{
		matView.eye.z += 0.5f;
		matView.target.z += 0.5f;
	}
	if (input->KDown(KEY::DOWN))
	{
		matView.eye.z -= 0.5f;
		matView.target.z -= 0.5f;
	}
	if (input->KDown(KEY::RIGHT))
	{
		matView.eye.x += 0.5f;
		matView.target.x += 0.5f;
	}
	if (input->KDown(KEY::LEFT))
	{
		matView.eye.x -= 0.5f;
		matView.target.x -= 0.5f;
	}

	XMFLOAT3 rot = sphereO.rotation;
	rot.y += 0.05f;
	sphereO.rotation = rot;

	matView.Update();

	domeObj.Update(matView.Get(), matPro->Get(), matView.eye);
	groundObj.Update(matView.Get(), matPro->Get(), matView.eye);
	sphereO.Update(matView.Get(), matPro->Get(), matView.eye);
}

void IF::Scene::Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT>viewport)
{
	graph.DrawBlendMode(commandList);
	domeObj.DrawBefore(commandList, graph.rootsignature.Get(), cb.GetGPUAddress());
	domeObj.Draw(commandList, viewport);

	groundObj.DrawBefore(commandList, graph.rootsignature.Get(), cb.GetGPUAddress());
	groundObj.Draw(commandList, viewport);

	sphereO.DrawBefore(commandList, graph.rootsignature.Get(), cb.GetGPUAddress());
	sphereO.Draw(commandList, viewport);

	//pgraph.DrawBlendMode(commandList, Blend::ADD);
	//tex->setTexture(commandList, efect);
	//fire->particle[0].DrawBefore(commandList, pgraph.rootsignature.Get(), tex->srvHeap.Get(), cb.GetGPUAddress(), D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	//fire->Draw(commandList, viewport);
}
