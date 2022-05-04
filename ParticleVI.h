#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <vector>
#include <wrl.h>

namespace IF
{
	struct VertexPos
	{
		using XMFLOAT3 = DirectX::XMFLOAT3;
		XMFLOAT3 pos;			//XYZç¿ïW
	};

	class PVI
	{
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT2 = DirectX::XMFLOAT2;
		template <class T> using vector = std::vector<T>;
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:
		ComPtr<ID3D12Resource> vertBuff = nullptr;

	public:
		vector<VertexPos> vertices;
		D3D12_VERTEX_BUFFER_VIEW vbView{};

	public:
		PVI(VertexPos* vertices, int vertexCount);
		HRESULT Initialize(ID3D12Device* device, ID3D12Resource* texBuff, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle);
	};
}