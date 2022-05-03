#include <DirectXMath.h>
#include "Window.h"
#include "DirectX12.h"
#include "Key.h"
#include "Object.h"
#include "Projection.h"
#include "View.h"
#include "Texture.h"
#include "RootParam.h"
#include "GPipeline.h"
#include "Graphic.h"
#include "VertexIndex.h"
#include "ConstBuff.h"

using namespace DirectX;
using namespace IF;


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {

	// ウィンドウサイズ
	const int winWidth = 1280;  // 横幅
	const int winHeight = 720;  // 縦幅

	Window* win = new Window(winWidth, winHeight);

#pragma region デバッグ
#ifdef _DEBUG
// デバックレイヤーをオンに
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif
#pragma endregion デバッグ

	HRESULT result;
	DirectX12* dx12 = new DirectX12(result, win->hwnd, winWidth, winHeight);
	Key* key = new Key(result, win->w.hInstance, win->hwnd);

#pragma region 描画初期化

	#pragma region 定数バッファの生成用の設定
	ConstBuff cb;
	result = cb.Initialize(dx12->device.Get());

	const size_t kObjectConst = 50;

	Object object3ds[kObjectConst];
	for (int i = 0; i < _countof(object3ds); i++)
	{
		result = object3ds[i].Initialize(dx12->device.Get());

		if (i > 0)
		{
			object3ds[i].parent = &object3ds[i - 1];
			object3ds[i].scale = { 0.9f,0.9f,0.9f };
			object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
			object3ds[i].position = { 0.0f,0.0f,-8.0f };
		}
	}

	#pragma region ビュー行列

	View matView;

#pragma endregion ビュー行列

	#pragma region 射影行列

	Projection matPro(45.0f, winWidth, winHeight);

#pragma endregion 射影行列

	Texture tex;

	tex.LoadTexture(L"Resources/texture.png", dx12->device.Get());

	for (int i = 0; i < _countof(object3ds); i++)
	{
		object3ds[i].VIInitialize(dx12->device.Get(), tex.texbuff.Get(), tex.srvHandle);
	}

	Graphic graph;
	result = graph.Initialize(dx12->device.Get(), tex.descRangeSRV);

#pragma endregion 描画初期化

	//変数宣言
	//-----------------------

	float angleY = 0.0f;
	float angleX = 0.0f;
	//-----------------------

	while (!key->Down(key->ESC))
	{
		//メッセージ
		if (win->Message())break;
		//キーアップデート
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
				//右
				if (key->Down(key->RIGHT))
				{
					object3ds[i].position.x += 1.0f;
				}
				//左
				if (key->Down(key->LEFT))
				{
					object3ds[i].position.x -= 1.0f;
				}
				//上
				if (key->Down(key->UP))
				{
					object3ds[i].position.y += 1.0f;
				}
				//下
				if (key->Down(key->DOWN))
				{
					object3ds[i].position.y -= 1.0f;
				}
			}
		}

		for (int i = 0; i < _countof(object3ds); i++)
		{
			object3ds[i].Update(matView.Get(), matPro.Get());
		}

		dx12->DrawBefore(graph.rootsignature.Get(), cb.GetGPUAddress(), tex.srvHeap);
		graph.DrawBlendMode(dx12->commandList.Get());

	#pragma region 描画コマンド
		for (int i = 0; i < _countof(object3ds); i++)
		{
			object3ds[i].Draw(dx12->commandList.Get(),dx12->viewport);
		}
#pragma endregion 描画コマンド

		dx12->DrawAfter();
	}

	delete key;
	delete dx12;
	delete win;

	return 0;
}