#pragma once
#include <d3d12.h>
#include <vector>

namespace IF
{
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;			//XYZ座標
		DirectX::XMFLOAT3 normal;		//法線ベクトル
		DirectX::XMFLOAT2 uv;			//UV座標
	};

	struct VertexPos
	{
		DirectX::XMFLOAT3 pos;			//XYZ座標
	};

	enum NormalFlag
	{
		NFALSE = false,
		NTRUE = true
	};

	class VI
	{
	public:
		virtual void SetVerticleIndex(std::vector<Vertex> vertices, int vertexCount, std::vector<unsigned short> indices, int indexCount) = 0;
		virtual HRESULT Initialize(ID3D12Device* device, NormalFlag flag = NFALSE) = 0;
		virtual D3D12_VERTEX_BUFFER_VIEW& GetVertexView() = 0;
		virtual D3D12_INDEX_BUFFER_VIEW& GetIndexView() = 0;
		virtual unsigned int GetSize() = 0;
	};
};