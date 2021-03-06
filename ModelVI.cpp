#include "ModelVI.h"

using namespace std;
using namespace DirectX;

void IF::MVI::SetVerticleIndex(std::vector<Vertex> vertices, size_t vertexCount, std::vector<unsigned short> indices, size_t indexCount)
{
	for (int i = 0; i < vertexCount; i++)
	{
		this->vertices.push_back(vertices[i]);
	}
	for (int i = 0; i < indexCount; i++)
	{
		this->indices.push_back(indices[i]);
	}
}

void IF::MVI::Initialize(ID3D12Device* device, bool smoothing, NormalFlag flag)
{
	HRESULT result;

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());


#pragma endregion 頂点データー

	//---------------------------

#pragma region 法線ベクトルの計算

	if (smoothing)
	{
		auto itr = smoothData.begin();
		for (; itr != smoothData.end(); ++itr)
		{
			vector<unsigned short>& v = itr->second;

			XMVECTOR normal = {};
			for (unsigned short index : v)
			{
				normal += XMVectorSet(vertices[index].normal.x, vertices[index].normal.y, vertices[index].normal.z, 0);
			}
			normal = XMVector3Normalize(normal / (float)v.size());
			for (unsigned short index : v)
			{
				vertices[index].normal = { normal.m128_f32[0],normal.m128_f32[1],normal.m128_f32[2] };
			}
		}
	}

	if (!indices.size() == 0 && flag == NTRUE)
	{
		for (int i = 0; i < indices.size() / 3; i++)
		{
			unsigned short index0 = indices[i * 3 + 0];
			unsigned short index1 = indices[i * 3 + 1];
			unsigned short index2 = indices[i * 3 + 2];

			XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);

			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);

			XMVECTOR normal = XMVector3Cross(v1, v2);

			normal = XMVector3Normalize(normal);

			XMStoreFloat3(&vertices[index0].normal, normal);
			XMStoreFloat3(&vertices[index1].normal, normal);
			XMStoreFloat3(&vertices[index2].normal, normal);
		}
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

	result = device->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE, &resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

#pragma endregion 頂点バッファの確保

	//---------------------------

#pragma region 頂点バッファへのデータ転送
// GPU上のバッファに対応した仮想メモリを取得
	Vertex* vertMap = nullptr;
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

	//-------------------------------

	if (!indices.size() == 0)
	{

#pragma region インデックスバッファの生成

		// インデックスデータ全体のサイズ
		UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices.size());
		// インデックスバッファの設定
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Width = sizeIB;//インデックスバッファの生成
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.SampleDesc.Count = 1;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		result = device->CreateCommittedResource(
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
		for (int i = 0; i < indices.size(); i++)
		{
			indexMap[i] = indices[i];	//インデックスをコピー
		}

		//つながりを解除
		indexBuff->Unmap(0, nullptr);

		ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibView.Format = DXGI_FORMAT_R16_UINT;
		ibView.SizeInBytes = sizeIB;

#pragma endregion インデックスバッファへのデータ転送

	}
}

D3D12_VERTEX_BUFFER_VIEW& IF::MVI::GetVertexView()
{
	return vbView;
}

D3D12_INDEX_BUFFER_VIEW& IF::MVI::GetIndexView()
{
	return ibView;
}

size_t IF::MVI::GetSize()
{
	return indices.size();
}

void IF::MVI::AddSmoothData(unsigned short indexPos, unsigned short indexVer)
{
	smoothData[indexPos].emplace_back(indexVer);
}
