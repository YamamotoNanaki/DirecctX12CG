#pragma once
#include <DirectXTex.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>

#pragma comment(lib,"d3d12.lib") 

class Texture
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
private:
	//WIC�e�N�X�`���̃��[�h
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	ScratchImage mipChain{};
	D3D12_HEAP_PROPERTIES texHeapProp{};
	D3D12_RESOURCE_DESC texresDesc{};
	ComPtr<ID3D12Resource> texbuff = nullptr;

public:
	void LoadTexture(const wchar_t* szFile, HRESULT result, ID3D12Device* device);
	void 
};