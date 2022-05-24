#pragma once
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <unordered_map>

namespace IF
{
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;			//XYZ���W
		DirectX::XMFLOAT3 normal;		//�@���x�N�g��
		DirectX::XMFLOAT2 uv;			//UV���W
	};
	enum NormalFlag
	{
		NFALSE = false,
		NTRUE = true
	};
	class MVI
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff = nullptr;
		std::vector<Vertex> vertices;
		std::unordered_map<unsigned short, std::vector<unsigned short>>smoothData;

	public:
		std::vector<unsigned short> indices;
		D3D12_VERTEX_BUFFER_VIEW vbView{};
		D3D12_INDEX_BUFFER_VIEW ibView{};

	public:
		void SetVerticleIndex(std::vector<Vertex> vertices, int vertexCount, std::vector<unsigned short> indices, int indexCount);
		HRESULT Initialize(ID3D12Device* device, bool smoothing, NormalFlag flag = NFALSE);
		D3D12_VERTEX_BUFFER_VIEW& GetVertexView();
		D3D12_INDEX_BUFFER_VIEW& GetIndexView();
		unsigned int GetSize();
		void AddSmoothData(unsigned short indexPos, unsigned short indexVer);
	};
}