#include "Scene.h"
#include "Input.h"
#include <DirectXMath.h>
#include "Rand.h"
#include "Light.h"
#include "MathConvert.h"

using namespace DirectX;
using namespace std;

IF::Scene::Scene(int winWidth, int winHeight, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport)
:winWidth(winWidth),winHeight(winHeight),device(device),commandList(commandList),viewport(viewport)
{
	Graphic::SetDevice(device);
	Texture::setDevice(device);
	Model::SetDevice(device);
}

IF::Scene::~Scene()
{
	delete matPro;
	delete light;
	sound->Reset();
	sound->SoundUnLoad(testSound);
}

void IF::Scene::Initialize()
{
	//音源
	testSound = sound->LoadWave("Resources/Alarm01.wav");

	//光源設定
	light = LightManager::GetInstance();
	light->Initialize();
	light->DefaultLightSetting();
	for (int i = 0; i < 3; i++)
	{
		light->SetDirLightActive(i, true);
		light->SetPointLightActive(i, false);
		light->SetSpotLightActive(i, false);
	}
	light->SetCircleShadowActive(0, true);
	light->SetAmbientColor({ 1, 1, 1 });
	Object::SetLight(light);
	//定数バッファの初期化
	cb.Initialize(device);

	//画像関連初期化
	graph.Initialize(tex->descRangeSRV, L"ModelVS.hlsl", L"ModelPS.hlsl", L"ModelGS.hlsl");
	graph.Initialize2D(tex->descRangeSRV, L"SpriteVS.hlsl", L"SpritePS.hlsl");

	//モデルの読み込みとオブジェクトとの紐付け(空と地面)
	tex->Initialize();
	domeM.LoadModel("skydome");
	groundM.LoadModel("ground");
	domeObj.Initialize(device, &domeM);
	groundObj.Initialize(device, &groundM);
	groundObj.position = { 0,-2,0 };

	//カメラ関連初期化
	matPro = new Projection(45.0f, (float)winWidth, (float)winHeight);
	matView.eye = { 1,1,-5.0f };

	//そのほかの初期化
	Rand::Initialize();

	sphereM.LoadModel("sphere", true);
	sphereO.Initialize(device, &sphereM);

	sphereO.position = { -1,0,0 };
	sphereO.scale = { 0.5,0.5,0.5 };
	matView.Update();


	//2D関連
	sprite.StaticInitialize(device, commandList, (float)winWidth, (float)winHeight);
	SGraph = tex->LoadTexture("Resources/kakuninn.png");
	sprite.Initialize(SGraph,{300,300});

	sound->SoundPlay(testSound);

	//デバッグ用
#ifdef _DEBUG
	dText.Initialize(tex->LoadTexture("Resources/debugfont.png"));

#endif // _DEBUG
}

void IF::Scene::Update()
{
	Input* input = Input::getInstance();
	input->Update();

	//光源
	static XMFLOAT3 dlColor = { 1,1,1 };
	static XMFLOAT3 spherePos = { -1,0,0 };

	//if (input->KDown(KEY::W))lightDir.m128_f32[1] += 1.0f;
	//if (input->KDown(KEY::S))lightDir.m128_f32[1] -= 1.0f;
	//if (input->KDown(KEY::D))lightDir.m128_f32[0] += 1.0f;
	//if (input->KDown(KEY::A))lightDir.m128_f32[0] -= 1.0f;

	light->SetCircleShadowDir(0, { csDir.x,csDir.y,csDir.z,0 });
	light->SetCircleShadowAtten(0, csAtten);
	light->SetCircleShadowFactorAngle(0, csAngle);

	for (int i = 0; i < 3; i++)light->SetDirLightColor(i, dlColor);

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

	if (input->KDown(KEY::W))spherePos.y += 0.5f;
	if (input->KDown(KEY::S))spherePos.y -= 0.5f;

	XMFLOAT3 rot = sphereO.rotation;
	rot.y += 0.05f;
	sphereO.rotation = rot;
	sphereO.position = spherePos;
	light->SetCircleShadowCasterPos(0, spherePos);

	matView.Update();
	light->Update();

	domeObj.Update(matView.Get(), matPro->Get(), matView.eye);
	groundObj.Update(matView.Get(), matPro->Get(), matView.eye);
	sphereO.Update(matView.Get(), matPro->Get(), matView.eye);

	sprite.position = { 540,500 };
	sprite.Update();

	//デバッグ用
#ifdef _DEBUG
	//dText.Print(100,100, 2, "matView.eye.x : %f",matView.eye.x);
#endif // _DEBUG
}

void IF::Scene::Draw()
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
	graph.DrawBlendMode(commandList, Blend::NORMAL2D);
	sprite.DrawBefore(graph.rootsignature.Get(), cb.GetGPUAddress());
	//sprite.Draw(viewport);

	//デバッグ用
#ifdef _DEBUG
	//dText.Draw(viewport);

#endif // _DEBUG
}
