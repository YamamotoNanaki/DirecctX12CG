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

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

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

#pragma region 頂点データ構造体
struct Vertex
{
	XMFLOAT3 pos;			//XYZ座標
	XMFLOAT3 normal;		//法線ベクトル
	XMFLOAT2 uv;			//UV座標
};
#pragma endregion 頂点データ構造体

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
		object3ds[i].Initialize(result,dx12->device.Get());

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

	#pragma region ルートパラメータの設定

	RootParam root(tex.descRangeSRV, 1);

#pragma endregion ルートパラメータの設定

	//-----------------------
	//頂点
	#pragma region 頂点データー
	//頂点データ
	Vertex vertices[] = {
		// x   y   z        u    v
		//前
		{{-5, -5, -5},{},{0.0f, 1.0f}},	//左下
		{{-5, +5, -5},{},{0.0f, 0.0f}},	//左上
		{{+5, -5, -5},{},{1.0f, 1.0f}},	//右下
		{{+5, +5, -5},{},{1.0f, 0.0f}},	//右上
		//後			
		{{+5, -5, +5},{},{1.0f, 1.0f}},	//右下
		{{+5, +5, +5},{},{1.0f, 0.0f}},	//右上
		{{-5, -5, +5},{},{0.0f, 1.0f}},	//左下
		{{-5, +5, +5},{},{0.0f, 0.0f}},	//左上
		//左			
		{{-5, -5, -5},{},{0.0f, 1.0f}},	//左下
		{{-5, -5, +5},{},{0.0f, 0.0f}},	//左上
		{{-5, +5, -5},{},{1.0f, 1.0f}},	//右下
		{{-5, +5, +5},{},{1.0f, 0.0f}},	//右上
		//右			
		{{+5, +5, -5},{},{1.0f, 1.0f}},	//右下
		{{+5, +5, +5},{},{1.0f, 0.0f}},	//右上
		{{+5, -5, -5},{},{0.0f, 1.0f}},	//左下
		{{+5, -5, +5},{},{0.0f, 0.0f}},	//左上
		//下			
		{{-5, +5, +5},{},{1.0f, 1.0f}},	//右下
		{{+5, +5, +5},{},{1.0f, 0.0f}},	//右上
		{{-5, +5, -5},{},{0.0f, 1.0f}},	//左下
		{{+5, +5, -5},{},{0.0f, 0.0f}},	//左上
		//上			
		{{-5, -5, -5},{},{0.0f, 1.0f}},	//左下
		{{+5, -5, -5},{},{0.0f, 0.0f}},	//左上
		{{-5, -5, +5},{},{1.0f, 1.0f}},	//右下
		{{+5, -5, +5},{},{1.0f, 0.0f}},	//右上
	};

	//インデックスデータ
	unsigned short indices[] = {
		//
		0,1,2,
		2,1,3,
		//
		4,5,6,
		6,5,7,
		//
		8,9,10,
		10,9,11,
		//
		12,13,14,
		14,13,15,
		//
		16,17,18,
		18,17,19,
		//
		20,21,22,
		22,21,23
	};

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));


#pragma endregion 頂点データー

	//---------------------------

	#pragma region 法線ベクトルの計算

	for (int i = 0; i < _countof(indices) / 3; i++)
	{
		//
		//
		unsigned short index0 = indices[i * 3 + 0];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];
		//
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//
		normal = XMVector3Normalize(normal);
		//
		XMStoreFloat3(&vertices[index0].normal, normal);
		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
	}

#pragma endregion 法線ベクトルの計算

	//---------------------------

	#pragma region 頂点バッファの確保
	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};   // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

	D3D12_RESOURCE_DESC resDesc{};  // リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの生成
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = dx12->device->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE, &resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

#pragma endregion 頂点バッファの確保

	//------------------------

	#pragma region シェーダーリソースビュー

	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};			//設定構造体
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	//RGBA
	srvDesc.Format = resDesc.Format;					//画像読み込み
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;		//2dテクスチャ
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	//ヒープの２番目にシェーダーリソースビュー作成
	dx12->device->CreateShaderResourceView(
		tex.texbuff.Get(),		//ビューと関連付けるバッファ
		&srvDesc,		//テクスチャ設定情報
		tex.srvHandle);

#pragma endregion シェーダーリソースビュー

	//---------------------------

	#pragma region 頂点バッファへのデータ転送
// GPU上のバッファに対応した仮想メモリを取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];   // 座標をコピー
	}

	// マップを解除
	vertBuff->Unmap(0, nullptr);

#pragma endregion 頂点バッファへのデータ転送

	//-----------------------------

	#pragma region 頂点バッファビューの作成
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

#pragma endregion 頂点バッファへビューの作成

	//-------------------------------

	#pragma region インデックスバッファの生成

// インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));
	// インデックスバッファの設定
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;//インデックスバッファの生成
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	result = dx12->device->CreateCommittedResource(
		&heapProp,				//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,				//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

#pragma endregion インデックスバッファの生成

	//----------------------------

	#pragma region インデックスバッファへのデータ転送
//GPU上のバッファに対応した仮想メモリを取得
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	//全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];	//インデックスをコピー
	}

	//つながりを解除
	indexBuff->Unmap(0, nullptr);


	D3D12_INDEX_BUFFER_VIEW ibView{};

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

#pragma endregion インデックスバッファへのデータ転送

	//------------------------------

	#pragma region 頂点シェーダーファイルの読み込みとコンパイル

	ComPtr<ID3DBlob> vsBlob = nullptr; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile( L"BasicVS.hlsl",  // シェーダファイル名
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0, &vsBlob, &errorBlob);


#pragma endregion 頂点シェーダーファイルの読み込みとコンパイル

	//-------------------------

	#pragma region シェーダーのエラー内容を表示
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

#pragma endregion シェーダーのエラー内容を表示

	//---------------------

	#pragma region ピクセルシェーダーの読み込みとコンパイル
// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile( L"BasicPS.hlsl",   // シェーダファイル名
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0, &psBlob, &errorBlob);


#pragma endregion ピクセルシェーダーの読み込みとコンパイル

	//------------------------

	#pragma region シェーダーのエラー内容を表示
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

#pragma endregion シェーダーのエラー内容を表示

	//---------------------

	#pragma region 頂点レイアウト
	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{// xyz座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{// 法線ベクトル
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{// uv座標
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

#pragma endregion 頂点レイアウト

	//------------------------

	#pragma region グラフィックスパイプライン設定

	GPipeline gPipeline(vsBlob.Get(), psBlob.Get(), inputLayout, _countof(inputLayout));


#pragma endregion グラフィックスパイプライン設定

	//-----------------

	#pragma region テクスチャサンプラーの設定

	D3D12_STATIC_SAMPLER_DESC samplerDesc{};

	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//横繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//縦繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//奥行繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//リニア補完
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									//ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;												//ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			//ピクセルシェーダーからのみ可視


#pragma endregion テクスチャサンプラーの設定

	//---------------------

	#pragma region ルートシグネチャの設定
	ComPtr<ID3D12RootSignature> rootsignature;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &root.rootParams.front();
	rootSignatureDesc.NumParameters = root.rootParams.size();
	//テクスチャ追加
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dx12->device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();

	gPipeline.RootSignature(*rootsignature.Get());
#pragma endregion ルートシグネチャの設定

	//----------------------

	#pragma region パイプラインステートの生成
	ComPtr<ID3D12PipelineState> pipelinestate = nullptr;
	result = dx12->device->CreateGraphicsPipelineState(&gPipeline.pipelineDesc[4], IID_PPV_ARGS(&pipelinestate));
	assert(SUCCEEDED(result));
#pragma endregion パイプラインステートの生成

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
		dx12->commandList->SetPipelineState(pipelinestate.Get());
		dx12->commandList->SetGraphicsRootSignature(rootsignature.Get());
#pragma endregion パイプラインステート

	#pragma region プリミティブ形状
		dx12->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#pragma endregion プリミティブ形状

	#pragma region 頂点バッファビューの設定コマンド
		dx12->commandList->IASetVertexBuffers(0, 1, &vbView);
#pragma endregion 頂点バッファビューの設定コマンド

	#pragma region インデックスバッファビューのセット
		dx12->commandList->IASetIndexBuffer(&ibView);
#pragma endregion インデックスバッファビューのセット

	#pragma region 定数バッファビューの設定コマンド

		dx12->commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
		dx12->commandList->SetDescriptorHeaps(1, &tex.srvHeap);
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = tex.srvHeap->GetGPUDescriptorHandleForHeapStart();
		dx12->commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

#pragma endregion 定数バッファビューの設定コマンド

	#pragma region 描画コマンド
		for (int i = 0; i < _countof(object3ds); i++)
		{
			object3ds[i].Draw(dx12->commandList.Get(), vbView, ibView, _countof(indices));
		}
#pragma endregion 描画コマンド

		dx12->DrawAfter(result);
	}

	return 0;
}