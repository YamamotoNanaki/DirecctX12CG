#pragma once
#include <DirectXTex.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>
#include <vector>

#pragma comment(lib,"d3d12.lib") 
namespace IF
{
	class Texture
	{
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		template<class T> using vector = std::vector<T>;
		using TexMetadata = DirectX::TexMetadata;
		using ScratchImage = DirectX::ScratchImage;
	private:
		//WIC�e�N�X�`���̃��[�h
		TexMetadata metadata{};
		ScratchImage scratchImg{};
		ScratchImage mipChain{};
		D3D12_HEAP_PROPERTIES texHeapProp{};
		D3D12_RESOURCE_DESC texresDesc{};
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

	public:
		ComPtr<ID3D12Resource> texbuff = nullptr;
		D3D12_DESCRIPTOR_RANGE descRangeSRV{};
		ID3D12DescriptorHeap* srvHeap = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;

	private:
		HRESULT TexLoad(const wchar_t* szFile);
		HRESULT LoadBuffer(ID3D12Device* device);
		void LoadTransfer(HRESULT result);
		HRESULT Heap(ID3D12Device* device);
		void Range();

	public:
		HRESULT LoadTexture(const wchar_t* szFile, ID3D12Device* device);
	};
}