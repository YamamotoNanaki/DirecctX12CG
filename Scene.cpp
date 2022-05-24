#include "Scene.h"
#include "Input.h"
#include <DirectXMath.h>
#include "Rand.h"

using namespace DirectX;

IF::Scene::Scene(float winWidth, float winHeight, HRESULT result, ID3D12Device* device)
{
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

	sphereM.LoadModel(device, "sphere");
	sphereO.Initialize(device, &sphereM);

	sphereO.position = { 0,1,0 };
	matView.Update();
}

IF::Scene::~Scene()
{
	delete matPro;
}

void IF::Scene::Update(ID3D12Device* device)
{
	Input* input = Input::getInstance();
	input->Update();


	domeObj.Update(matView.Get(), matPro->Get());
	groundObj.Update(matView.Get(), matPro->Get());
	sphereO.Update(matView.Get(), matPro->Get());
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
