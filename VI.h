#pragma once
#include <d3d12.h>
#include <vector>
#include "IFMath.h"

namespace IF
{
	struct Vertex
	{
		Float3 pos;			//XYZ���W
		Float3 normal;		//�@���x�N�g��
		Float2 uv;			//UV���W
	};

	struct VertexPos
	{
		Float3 pos;			//XYZ���W
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