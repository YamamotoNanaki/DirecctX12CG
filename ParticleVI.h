#pragma once
#include <vector>
#include <wrl.h>
#include "VI.h"

namespace IF
{
	class PV :public VI
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;

	public:
		std::vector<VertexPos> vertices;
		D3D12_VERTEX_BUFFER_VIEW vbView{};

	public:
		void SetVerticleIndex(std::vector<Vertex> vertices, int vertexCount, std::vector<unsigned short> indices, int indexCount)override;
		HRESULT Initialize(ID3D12Device* device, NormalFlag flag)override;
		virtual D3D12_VERTEX_BUFFER_VIEW& GetVertexView()override;
		virtual unsigned int GetSize()override;

	private:
		virtual D3D12_INDEX_BUFFER_VIEW& GetIndexView()override;
	};
}