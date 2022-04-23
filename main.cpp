#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <wrl.h>
#include "DxWindow.h"
#include "Dx12.h"
#include "Key.h"

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

#pragma comment(lib,"d3d12.lib") 
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#pragma region 定数バッファ構造体

struct ConstBufferDataMaterial
{
	XMFLOAT4 color;	//色(RGBA)
};

struct ConstBufferDataTransform
{
	XMMATRIX mat;
};
#pragma endregion 定数バッファ構造体

#pragma region 頂点データ構造体
struct Vertex
{
	XMFLOAT3 pos;			//XYZ座標
	//XMFLOAT3 normal;		//法線ベクトル
	XMFLOAT2 uv;			//UV座標
};
#pragma endregion 頂点データ構造体

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {

	//--- ウインドウの生成処理 -----------------------------------------------------//

// ウィンドウの生成（前処理）
#pragma region ウィンドウの生成（前処理）

	// ウィンドウサイズ
	const int window_width = 1280;  // 横幅
	const int window_height = 720;  // 縦幅

	DxWindow* win = new DxWindow(window_width, window_height);

#pragma endregion ウィンドウの生成（前処理）

	//------------------------------------------------------------------------------//

	MSG msg{}; // メッセージ

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

	Dx12* dx12 = new Dx12(result, win->hwnd, window_width, window_height);

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
	ComPtr<ID3D12Resource> constBuffTransform = nullptr;
	result = dx12->device->CreateCommittedResource(
		&cbHeapProp,	//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,		//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial)
	);
	assert(SUCCEEDED(result));
	result = dx12->device->CreateCommittedResource(
		&cbHeapProp,	//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,		//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform)
	);
	assert(SUCCEEDED(result));

#pragma endregion 定数バッファの生成用の設定

	//-------------------------

	#pragma region 定数バッファにデータを転送する
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	ConstBufferDataTransform* constMapTransform = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);	//マッピング
	assert(SUCCEEDED(result));
	result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);	//マッピング
	assert(SUCCEEDED(result));
	constMapMaterial->color = XMFLOAT4(1, 1, 1, 1);					//RGBAで半透明の赤
	constBuffMaterial->Unmap(0, nullptr);							//マッピング解除

	//行列
	constMapTransform->mat = XMMatrixIdentity();	//単位行列
	constMapTransform->mat = XMMatrixOrthographicOffCenterLH(0, window_width, window_height, 0, 0, 1);

#pragma endregion 定数バッファにデータを転送する

	//----------------------

	#pragma region ビュー行列

	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);
	XMFLOAT3 target(0, 0, 0);
	XMFLOAT3 up(0, 1, 0);
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

#pragma endregion ビュー行列

	#pragma region 射影行列

	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f), (float)window_width / window_height, 0.1f, 1000.0f);

#pragma endregion 射影行列

	#pragma region ワールド行列

	XMMATRIX matWorld;
	matWorld = XMMatrixIdentity();
		#pragma region スケーリング

	XMMATRIX matScale;		//スケーリング行列
	matScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	matWorld *= matScale;	//ワールド行列にスケーリングを反映

		#pragma endregion スケーリング

		#pragma region 回転

	XMMATRIX matRot;		//回転行列
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(0));	//Z軸まわりに45度回転
	matRot *= XMMatrixRotationX(XMConvertToRadians(0));	//X軸まわりに45度回転
	matRot *= XMMatrixRotationY(XMConvertToRadians(0));	//Y軸まわりに45度回転
	matWorld *= matRot;		//ワールド行列に回転を反映

		#pragma endregion 回転

		#pragma region 平行移動

	XMMATRIX matTrans;	//平行移動行列
	matTrans = XMMatrixTranslation(0, 0, 0);	//(50,0,0)平行移動
	matWorld *= matTrans;	//ワールド行列に平行移動を反映

	XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
	XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };		//座標を変数として持つ

		#pragma endregion 平行移動

	constMapTransform->mat = matWorld * matView * matProjection;
#pragma endregion ワールド行列

	//-------------------------

	#pragma region 画像イメージデータの作成

	//WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		L"Resources/texture.png",			//Data\Resources\texture.pngを指定
		WIC_FLAGS_NONE, &metadata, scratchImg);

	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	metadata.format = MakeSRGB(metadata.format);

	//const Image* img = scratchImg.GetImage(0, 0, 0);	//生データ抽出

	//const size_t textureWidth = 256;
	//const size_t textureHeight = 256;
	//const size_t imageDataCount = textureWidth * textureHeight;

	//XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];

	//	for (int i = 0; i < imageDataCount; i++)
	//	{
	//		imageData[i].x = 1.0f;
	//		imageData[i].y = 0.0f;
	//		imageData[i].z = 0.0f;
	//		imageData[i].w = 1.0f;
	//	}
#pragma endregion 画像イメージデータの作成

	#pragma region テクスチャバッファの生成

		D3D12_HEAP_PROPERTIES texHeapProp{};	//テクスチャヒープ設定
		texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

		//D3D12_RESOURCE_DESC texresDesc{};		//テクスチャ設定
		//texresDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//2dテクスチャ用
		//texresDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			//RGBAフォーマット
		//texresDesc.Width = textureWidth;			//幅
		//texresDesc.Height = textureHeight;			//高さ
		//texresDesc.DepthOrArraySize = 1;
		//texresDesc.MipLevels = 1;
		//texresDesc.SampleDesc.Count = 1;

		//画像読み込み
		D3D12_RESOURCE_DESC texresDesc{};		//テクスチャ設定
		//texresDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
		texresDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texresDesc.Format = metadata.format;
		texresDesc.Width = metadata.width;			//幅
		texresDesc.Height = (UINT)metadata.height;	//高さ
		texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
		texresDesc.MipLevels = (UINT16)metadata.mipLevels;
		texresDesc.SampleDesc.Count = 1;

		ComPtr<ID3D12Resource> texbuff = nullptr;
		result = dx12->device->CreateCommittedResource(		//GPUリソースの生成
			&texHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,		//テクスチャ用指定
			nullptr,
			IID_PPV_ARGS(&texbuff));

#pragma endregion テクスチャバッファの生成

	#pragma region テクスチャバッファへのデータ転送
		
		for (size_t i = 0; i < metadata.mipLevels; i++)
		{
			const Image* img = scratchImg.GetImage(i, 0, 0);
			//テクスチャバッファにデータ転送
			result = texbuff->WriteToSubresource(
				(UINT)i,
				nullptr,				//全領域へコピー
				img->pixels,			//元データアドレス
				//sizeof(XMFLOAT4) * textureWidth,
				//sizeof(XMFLOAT4) * imageDataCount
				(UINT)img->rowPitch,	//1ラインサイズ
				(UINT)img->slicePitch	//1枚サイズ
			);
			assert(SUCCEEDED(result));
		}

		//元データ解放（忘れずに解放）
		//delete[] imageData;

#pragma endregion テクスチャバッファへのデータ転送

	//-------------------------

	#pragma region デスクリプタヒープ生成

		const size_t kMaxSRVCount = 2056;

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeapDesc.NumDescriptors = kMaxSRVCount;

		ID3D12DescriptorHeap* srvHeap = nullptr;
		result = dx12->device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
		assert(SUCCEEDED(result));

		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
#pragma endregion デスクリプタヒープ生成

	//-------------------------

	#pragma region デスクリプタレンジの設定

		D3D12_DESCRIPTOR_RANGE descRangeSRV{};														//テクスチャ用
		descRangeSRV.NumDescriptors = 1;															//テクスチャ一つ
		descRangeSRV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;									//種別はテクスチャ
		descRangeSRV.BaseShaderRegister = 0;														//0番スロットから
		descRangeSRV.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

#pragma endregion デスクリプタレンジの設定

	//----------------------------

	#pragma region ルートパラメータの設定

	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//定数用
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[0].Descriptor.ShaderRegister = 0;								//デスクリプタレンジ
	rootParams[0].Descriptor.RegisterSpace = 0;									//デスクリプタレンジ数
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//すべてのシェーダーから見える
	//テクスチャ用
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descRangeSRV;				//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//すべてのシェーダーから見える
	//定数バッファ2
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[2].Descriptor.ShaderRegister = 1;								//デスクリプタレンジ
	rootParams[2].Descriptor.RegisterSpace = 0;									//デスクリプタレンジ数
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//すべてのシェーダーから見える

#pragma endregion ルートパラメータの設定

	//-----------------------
	//頂点
	#pragma region 頂点データー
	//頂点データ
	Vertex vertices[] = {
		//    x       y      z      u    v
		//前
		{{-50, -50, +0},{0.0f, 1.0f}},	//左下
		{{-50, +50, +0},{0.0f, 0.0f}},	//左上
		{{+50, -50, +0},{1.0f, 1.0f}},	//右下
		{{+50, +50, +0},{1.0f, 0.0f}},	//右上
		////後
		//{{-5, -5, +5},{0.0f, 1.0f}},	//左下
		//{{-5, +5, +5},{0.0f, 0.0f}},	//左上
		//{{+5, -5, +5},{1.0f, 1.0f}},	//右下
		//{{+5, +5, +5},{1.0f, 0.0f}},	//右上
		////左
		//{{-5, -5, -5},{0.0f, 1.0f}},	//左下
		//{{-5, -5, +5},{0.0f, 0.0f}},	//左上
		//{{-5, +5, -5},{1.0f, 1.0f}},	//右下
		//{{-5, +5, +5},{1.0f, 0.0f}},	//右上
		////右
		//{{+5, -5, -5},{0.0f, 1.0f}},	//左下
		//{{+5, -5, +5},{0.0f, 0.0f}},	//左上
		//{{+5, +5, -5},{1.0f, 1.0f}},	//右下
		//{{+5, +5, +5},{1.0f, 0.0f}},	//右上
		////下
		//{{-5, +5, -5},{0.0f, 1.0f}},	//左下
		//{{+5, +5, -5},{0.0f, 0.0f}},	//左上
		//{{-5, +5, +5},{1.0f, 1.0f}},	//右下
		//{{+5, +5, +5},{1.0f, 0.0f}},	//右上
		////上
		//{{-5, -5, -5},{0.0f, 1.0f}},	//左下
		//{{+5, -5, -5},{0.0f, 0.0f}},	//左上
		//{{-5, -5, +5},{1.0f, 1.0f}},	//右下
		//{{+5, -5, +5},{1.0f, 0.0f}},	//右上
	};

	//インデックスデータ
	unsigned short indices[] = {
		//
		0,1,2,
		2,1,3,
		////
		//4,5,6,
		//5,6,7,
		////
		//8,9,10,
		//9,10,11,
		////
		//12,13,14,
		//13,14,15,
		////
		//16,17,18,
		//17,18,19,
		////
		//20,21,22,
		//21,22,23
	};

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));


#pragma endregion 頂点データー

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
		texbuff.Get(),		//ビューと関連付けるバッファ
		&srvDesc,		//テクスチャ設定情報
		srvHandle);

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
		{// uv座標
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

#pragma endregion 頂点レイアウト

	//------------------------

	#pragma region グラフィックスパイプライン設定
// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

		#pragma region デプスステンシルステートの設定

	////デプスステンシルステートの設定
	//pipelineDesc.DepthStencilState.DepthEnable = true;		//深度テストを行う
	//pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//書き込み許可
	//pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	//pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;		//深度値フォーマット

		#pragma endregion デプスステンシルステートの設定

	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;  // カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	//ブレンド設定
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	blenddesc.BlendEnable = true;						//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;		//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;			//ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;		//デストの値を  0%使う

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//1.0f-ソースのアルファ値

	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

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
	rootSignatureDesc.pParameters = rootParams;
	rootSignatureDesc.NumParameters = _countof(rootParams);
	//テクスチャ追加
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dx12->device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();

	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootsignature.Get();
#pragma endregion ルートシグネチャの設定

	//----------------------

	#pragma region パイプラインステートの生成
	ComPtr<ID3D12PipelineState> pipelinestate = nullptr;
	result = dx12->device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelinestate));
	assert(SUCCEEDED(result));
#pragma endregion パイプラインステートの生成

#pragma endregion 描画初期化

	//変数宣言
	//-----------------------

	float angle = 0.0f;
	//-----------------------



	while (!key->Down(key->ESC))
	{
#pragma region メッセージ
		// メッセージがある？
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); // キー入力メッセージの処理
			DispatchMessage(&msg);  //プロシージャにメッセージを送る
		}

		//Xボタンで終了メッセージが来たらゲームループを抜ける
		if (msg.message == WM_QUIT)
		{
			break;
		}

#pragma endregion メッセージ

#pragma region DirectInput

		key->Update(result);

#pragma endregion DirectInput


		/*if (key->Down(key->D) || key->Down(key->A))
		{
			if (key->Down(key->D))angle += XMConvertToRadians(1.0f);
			if (key->Down(key->A))angle -= XMConvertToRadians(1.0f);

			eye.x = -100 * sinf(angle);
			eye.z = -100 * cosf(angle);
		}*/

		eye.z -= 1;

		matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

		if (key->Judge(key->Arrow, 4, key->OR))
		{
			for (int i = 0; i < 4; i++)
			{
				//右
				if (key->Down(key->RIGHT))
				{
					position.x += 1;
				}
				//左
				if (key->Down(key->LEFT))
				{
					position.x -= 1;
				}
				//上
				if (key->Down(key->UP))
				{
					position.z += 1;
				}
				//下
				if (key->Down(key->DOWN))
				{
					position.z -= 1;
				}
			}


		}

		matTrans = XMMatrixTranslation(position.x, position.y, position.z);	//平行移動行列を再計算
		matRot *= XMMatrixRotationZ(XMConvertToRadians(0.5f));

		matWorld = XMMatrixIdentity();		//ワールド行列は毎フレームリセット

		matWorld *= matTrans;				//ワールド行列に平行移動を反映
		matWorld *= matRot;

		constMapTransform->mat = matWorld * matView * matProjection;


#pragma region DirectX毎フレームの処理 前

	#pragma region リソースバリアの変更
		//バックバッファの番号を取得（2つなので0番か1番）
		UINT bbIndex = dx12->swapchain->GetCurrentBackBufferIndex();

		// 1．リソースバリアで書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = dx12->backBuffers[bbIndex].Get(); // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画
		dx12->commandList->ResourceBarrier(1, &barrierDesc);
#pragma endregion リソースバリアの変更

	#pragma region 描画先指定
		// 2．描画先指定
		// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = dx12->rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * dx12->device->GetDescriptorHandleIncrementSize(dx12->rtvHeapDesc.Type);
		//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
		//D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		dx12->commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr/*&dsvH*/);

#pragma endregion 描画先指定

	#pragma region 画面クリア
		// 3．画面クリア           R     G     B    A
		float clearColor[4] = {0.1f ,0.25f ,0.5f ,1.0f};

		dx12->commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		/*cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);*/
#pragma endregion 画面クリア

#pragma endregion DirectX毎フレームの処理 前

	#pragma region ビューポート設定コマンド
		D3D12_VIEWPORT viewport{};

		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		dx12->commandList->RSSetViewports(1, &viewport);
#pragma endregion ビューポート設定コマンド5

	#pragma region シザー矩形の設定コマンド
		D3D12_RECT scissorrect{};

		scissorrect.left = 0;
		scissorrect.right = scissorrect.left + window_width;
		scissorrect.top = 0;
		scissorrect.bottom = scissorrect.top + window_height;

		dx12->commandList->RSSetScissorRects(1, &scissorrect);
#pragma endregion シザー矩形の設定コマンド

	#pragma region パイプラインステート
		dx12->commandList->SetPipelineState(pipelinestate.Get());
		dx12->commandList->SetGraphicsRootSignature(rootsignature.Get());

		////デスクリプタヒープを設定
		//ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap };
		//cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		////定数バッファビューをセット
		////cmdList->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());

		////テクスチャ追加
		////デスクリプタヒープの先頭ハンドルを取得(GPU版)
		//D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		////ヒープの先頭にあるCBVをルートパラメータ0番に設定
		//cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandle);
		////ハンドルを1つ進める
		//gpuDescHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		////ヒープの2番目にあるSRVをルートパラメータを1番に設定
		//cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandle);

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
		dx12->commandList->SetDescriptorHeaps(1, &srvHeap);
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
		dx12->commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		dx12->commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

#pragma endregion 定数バッファビューの設定コマンド

	#pragma region 描画コマンド
		dx12->commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);
#pragma endregion 描画コマンド

#pragma region DirectX毎フレームの処理 後

	#pragma region リソースバリアの復帰コマンド
// 5．リソースバリアを戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;   // 表示に
		dx12->commandList->ResourceBarrier(1, &barrierDesc);
#pragma endregion リソースバリアの復帰コマンド

	#pragma region ためておいた命令を実行する
		// 命令のクローズ
		result = dx12->commandList->Close();
		assert(SUCCEEDED(result));
		// コマンドリストの実行
		ID3D12CommandList* commandLists[] = { dx12->commandList.Get()}; // コマンドリストの配列
		dx12->commandQueue->ExecuteCommandLists(1, commandLists);

		// バッファをフリップ（裏表の入替え）
		result = dx12->swapchain->Present(1, 0);
		assert(SUCCEEDED(result));
#pragma endregion ためておいた命令を実行する

	#pragma region 画面入れ替え
		// コマンドリストの実行完了を待つ
		dx12->commandQueue->Signal(dx12->fence.Get(), ++dx12->fenceVal);

		if (dx12->fence->GetCompletedValue() != dx12->fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			dx12->fence->SetEventOnCompletion(dx12->fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		result = dx12->commandAllocator->Reset(); // キューをクリア
		assert(SUCCEEDED(result));
		result = dx12->commandList->Reset(dx12->commandAllocator.Get(), nullptr);  // 再びコマンドリストを貯める準備
		assert(SUCCEEDED(result));
#pragma endregion 画面入れ替え

#pragma endregion DirectX毎フレームの処理 後
	}

	delete key;
	delete win;

	return 0;
}