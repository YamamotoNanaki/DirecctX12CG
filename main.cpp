#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <wrl.h>
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

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;
using namespace IF;

#pragma comment(lib,"d3d12.lib") 
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxguid.lib")

#pragma region 定数バッファ構造体

struct ConstBufferDataMaterial
{
	XMFLOAT4 color;	//色(RGBA)
};

#pragma endregion 定数バッファ構造体


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {

	//--- ウインドウの生成処理 -----------------------------------------------------//

// ウィンドウの生成（前処理）
#pragma region ウィンドウの生成（前処理）

	// ウィンドウサイズ
	const int winWidth = 1280;  // 横幅
	const int winHeight = 720;  // 縦幅

	Window* win = new Window(winWidth, winHeight);

#pragma endregion ウィンドウの生成（前処理）

	//------------------------------------------------------------------------------//


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
//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//GPUリソースの生成
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;
	result = dx12->device->CreateCommittedResource(
		&cbHeapProp,	//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,		//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial)
	);

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

#pragma endregion 定数バッファの生成用の設定

	//-------------------------

	#pragma region 定数バッファにデータを転送する
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);	//マッピング
	assert(SUCCEEDED(result));
	constMapMaterial->color = XMFLOAT4(1, 1, 1, 1);					//RGBAで半透明の赤
	constBuffMaterial->Unmap(0, nullptr);							//マッピング解除


#pragma endregion 定数バッファにデータを転送する

	//----------------------

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
	float color = 0;
	//-----------------------



	while (!key->Down(key->ESC))
	{
		//メッセージ
		if (win->Message())break;
		//キーアップデート
		key->Update(result);


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

		color+=0.005;
		if (color >= 1.0f)
		{
			color = 0;
		}
		//result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);	//マッピング
		//assert(SUCCEEDED(result));
		//constMapMaterial->color = XMFLOAT4(color, 0, 0, 1);					//RGBAで半透明の赤
		//constBuffMaterial->Unmap(0, nullptr);							//マッピング解除


		dx12->DrawBefore();

	#pragma region ビューポート設定コマンド
		D3D12_VIEWPORT viewport{};

		viewport.Width = winWidth;
		viewport.Height = winHeight;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		dx12->commandList->RSSetViewports(1, &viewport);
#pragma endregion ビューポート設定コマンド5

	#pragma region シザー矩形の設定コマンド
		D3D12_RECT scissorrect{};

		scissorrect.left = 0;
		scissorrect.right = scissorrect.left + winWidth;
		scissorrect.top = 0;
		scissorrect.bottom = scissorrect.top + winHeight;

		dx12->commandList->RSSetScissorRects(1, &scissorrect);
#pragma endregion シザー矩形の設定コマンド

	#pragma region パイプラインステート
		dx12->commandList->SetGraphicsRootSignature(graph.rootsignature.Get());
#pragma endregion パイプラインステート

	#pragma region プリミティブ形状
		dx12->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#pragma endregion プリミティブ形状


	#pragma region 定数バッファビューの設定コマンド

		dx12->commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
		dx12->commandList->SetDescriptorHeaps(1, &tex.srvHeap);
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = tex.srvHeap->GetGPUDescriptorHandleForHeapStart();
		dx12->commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

#pragma endregion 定数バッファビューの設定コマンド
			dx12->commandList->SetPipelineState(graph.pipelinestate[0].Get());

	#pragma region 描画コマンド
		for (int i = 0; i < _countof(object3ds); i++)
		{
			object3ds[i].Draw(dx12->commandList.Get());
		}
#pragma endregion 描画コマンド

		dx12->DrawAfter(result);
	}

	return 0;
}