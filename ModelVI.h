#pragma once
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>
#include "VI.h"

namespace IF
{
	class MVI : public VI
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff = nullptr;
		std::vector<Vertex> vertices;

	public:
		std::vector<unsigned short> indices;
		D3D12_VERTEX_BUFFER_VIEW vbView{};
		D3D12_INDEX_BUFFER_VIEW ibView{};

	public:
		void SetVerticleIndex(std::vector<Vertex> vertices, int vertexCount, std::vector<unsigned short> indices, int indexCount)override;
		HRESULT Initialize(ID3D12Device* device, NormalFlag flag)override;
		virtual D3D12_VERTEX_BUFFER_VIEW& GetVertexView()override;
		virtual D3D12_INDEX_BUFFER_VIEW& GetIndexView()override;
		virtual unsigned int GetSize()override;
	};
}