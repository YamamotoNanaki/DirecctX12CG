#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <vector>
#include <wrl.h>

namespace IF
{
	struct Vertex
	{
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT2 = DirectX::XMFLOAT2;
		XMFLOAT3 pos;			//XYZ座標
		XMFLOAT3 normal;		//法線ベクトル
		XMFLOAT2 uv;			//UV座標
	};

	class objVI
	{
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT2 = DirectX::XMFLOAT2;
		template <class T> using vector = std::vector<T>;
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:
		vector<Vertex> vertices;
		ComPtr<ID3D12Resource> vertBuff = nullptr;
		ComPtr<ID3D12Resource> indexBuff = nullptr;

	public:
		vector<unsigned short> indices;
		D3D12_VERTEX_BUFFER_VIEW vbView{};
		D3D12_INDEX_BUFFER_VIEW ibView{};

	public:
		objVI(vector<Vertex> vertices, int vertexCount, vector<unsigned short> indices, int indexCount);
		objVI(Vertex* vertices, int vertexCount, unsigned short* indices, int indexCount);
		HRESULT Initialize(ID3D12Device* device, ID3D12Resource* texBuff, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle);
	};
}