#include "Model.h"
#include "View.h"
#include <d3dx12.h>
#include <fstream>
#include <sstream>
#include <string>
#include <DirectXTex.h>


using namespace DirectX;
using namespace IF;
using namespace std;

HRESULT Model::Initialize(ID3D12Device* device)
{
	HRESULT result;
	tex.Initialize(device);
	graph.Initialize(device, tex.descRangeSRV, 3, L"ModelVS.hlsl", L"ModelPS.hlsl", L"ModelGS.hlsl");
	//定数バッファのヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//定数バッファのリソース設定
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//定数バッファの生成

	result = device->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE, &resdesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuffTransform0));
	assert(SUCCEEDED(result));

	result = device->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE, &resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuffTransform1));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffTransform0->Map(0, nullptr, (void**)&constMapTransform);
	result = constBuffTransform1->Map(0, nullptr, (void**)&constMapMaterial);
	constMapMaterial->ambient = material.ambient;
	constMapMaterial->diffuse = material.diffuse;
	constMapMaterial->specular = material.specular;
	constMapMaterial->alpha = material.alpha;
	assert(SUCCEEDED(result));

	const string modelname = "test";
	const string filename = modelname + ".obj";
	const string directory = "Resources/" + modelname + "/";

	ifstream file;
	file.open(directory + filename);
	if (file.fail())
	{
		assert(0);
	}

	vector<XMFLOAT3> position;
	vector<XMFLOAT3> normal;
	vector<XMFLOAT2> texcode;

	vector<Vertex> vertices;
	vector<unsigned short> indices;

	string line;
	while (getline(file, line))
	{
		istringstream line_stream(line);

		string key;
		getline(line_stream, key, ' ');

		if (key == "v")
		{
			XMFLOAT3 pos{};
			line_stream >> pos.x;
			line_stream >> pos.y;
			line_stream >> pos.z;
			position.emplace_back(pos);
		}
		if (key == "vn")
		{
			XMFLOAT3 nor{};
			line_stream >> nor.x;
			line_stream >> nor.y;
			line_stream >> nor.z;
			normal.emplace_back(nor);
		}
		if (key == "vt")
		{
			XMFLOAT2 tex{};
			line_stream >> tex.x;
			line_stream >> tex.y;
			tex.y = 1 - tex.y;
			texcode.emplace_back(tex);
		}
		if (key == "f")
		{
			string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				istringstream index_stream(index_string);
				unsigned short inP, inT, inN;
				index_stream >> inP;
				index_stream.seekg(1, ios_base::cur);
				index_stream >> inT;
				index_stream.seekg(1, ios_base::cur);
				index_stream >> inN;

				Vertex vertex{};
				vertex.pos = position[inP - 1];
				vertex.normal = normal[inN - 1];
				vertex.uv = texcode[inT - 1];
				vertices.emplace_back(vertex);

				indices.emplace_back((unsigned short)indices.size());
			}
		}
		if (key == "mtllib")
		{
			string filename;
			line_stream >> filename;
			LoadMaterial(directory, filename);
		}
	}

	file.close();

	vi = new objVI(vertices, vertices.size(), indices, indices.size());

	return result;
}

HRESULT Model::VIInitialize(ID3D12Device* device)
{
	HRESULT result = vi->Initialize(device, tex.texbuff.Get(), tex.srvHandle);
	return result;
}

void IF::Model::DrawBefore(ID3D12GraphicsCommandList* commandList, D3D12_GPU_VIRTUAL_ADDRESS GPUAddress, D3D_PRIMITIVE_TOPOLOGY topology)
{
	commandList->SetGraphicsRootSignature(graph.rootsignature.Get());
	commandList->IASetPrimitiveTopology(topology);
	commandList->SetGraphicsRootConstantBufferView(0, GPUAddress);
	commandList->SetDescriptorHeaps(1, &tex.srvHeap);
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = tex.srvHeap->GetGPUDescriptorHandleForHeapStart();
	commandList->SetGraphicsRootDescriptorTable(2, srvGpuHandle);
}

void Model::Update(XMMATRIX matView, XMMATRIX matProjection, BillBoardMode mode)
{
	XMMATRIX matScale, matRot, matTrams;

	//スケール、回転、平行移動
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(rotation.z);
	matRot *= XMMatrixRotationZ(rotation.x);
	matRot *= XMMatrixRotationZ(rotation.y);
	matTrams = XMMatrixTranslation(position.x, position.y, position.z);
	//ワールド行列の合成
	matWorld = XMMatrixIdentity();
	if (mode == BILLBOARD)matWorld *= View::matBillBoard;
	else if (mode == YBOARD)matWorld *= View::matBillBoardY;
	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrams;
	//親オブジェクトがあれば
	if (parent != nullptr)
	{
		matWorld *= parent->matWorld;
	}

	//定数バッファへのデータ転送
	constMapTransform->mat = matWorld * matView * matProjection;
}

void Model::Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport)
{
	for (int i = 0; i < viewport.size(); i++)
	{
		commandList->RSSetViewports(1, &viewport[i]);
		//頂点バッファの設定
		commandList->IASetVertexBuffers(0, 1, &vi->vbView);
		//インデックスバッファの設定
		commandList->IASetIndexBuffer(&vi->ibView);
		//定数バッファビューの設定
		commandList->SetGraphicsRootConstantBufferView(0, constBuffTransform0->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(1, constBuffTransform1->GetGPUVirtualAddress());
		//描画コマンド
		commandList->DrawIndexedInstanced(vi->indices.size(), 1, 0, 0, 0);
	}
}

Model::~Model()
{
	delete vi;
}

void IF::Model::LoadMaterial(const std::string& directory, const std::string& filename)
{
	ifstream file;
	file.open(directory + filename);
	if (file.fail())assert(0);

	string line;
	while (getline(file, line))
	{
		istringstream line_stream(line);
		string key;
		getline(line_stream, key, ' ');

		if (key[0] == '\t')
		{
			key.erase(key.begin());
		}
		if (key == "newmtl")
		{
			line_stream >> material.name;
		}
		if (key == "Ka")
		{
			line_stream >> material.ambient.x;
			line_stream >> material.ambient.y;
			line_stream >> material.ambient.z;
		}
		if (key == "Kd")
		{
			line_stream >> material.diffuse.x;
			line_stream >> material.diffuse.y;
			line_stream >> material.diffuse.z;
		}
		if (key == "Ks")
		{
			line_stream >> material.specular.x;
			line_stream >> material.specular.y;
			line_stream >> material.specular.z;
		}
		if (key == "map_Kd")
		{
			line_stream >> material.textureFilename;
			LoadTexture(directory, material.textureFilename);
		}
	}
	file.close();
}

bool IF::Model::LoadTexture(const std::string& directory, const std::string& filename)
{
	string filepath = directory + filename;

	wchar_t wfilepath[128];
	int iBSize = MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), -1, wfilepath, _countof(wfilepath));
	HRESULT result = tex.Loadtexture(wfilepath);

	return result;
}

void IF::Model::DrawBlendMode(ID3D12GraphicsCommandList* commandList, Blend blend)
{
	graph.BlendMode(commandList, blend);
}
