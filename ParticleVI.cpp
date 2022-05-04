#include "ParticleVI.h"

using namespace DirectX;
using namespace IF;

PVI::PVI(VertexPos* vertices, int vertexCount)
{
	for (int i = 0; i < vertexCount; i++)
	{
		this->vertices.push_back(vertices[i]);
	}
}

HRESULT PVI::Initialize(ID3D12Device* device, ID3D12Resource* texBuff, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle)
{
	HRESULT result;

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());


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

	result = device->CreateCommittedResource(
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
	device->CreateShaderResourceView(
		texBuff,		//ビューと関連付けるバッファ
		&srvDesc,		//テクスチャ設定情報
		srvHandle);

#pragma endregion シェーダーリソースビュー

	//---------------------------

#pragma region 頂点バッファへのデータ転送
// GPU上のバッファに対応した仮想メモリを取得
	VertexPos* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	// 全頂点に対して
	for (int i = 0; i < vertices.size(); i++)
	{
		vertMap[i] = vertices[i];   // 座標をコピー
	}

	// マップを解除
	vertBuff->Unmap(0, nullptr);

#pragma endregion 頂点バッファへのデータ転送

	//-----------------------------

#pragma region 頂点バッファビューの作成
// 頂点バッファビューの作成

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

#pragma endregion 頂点バッファへビューの作成

	return result;
}
