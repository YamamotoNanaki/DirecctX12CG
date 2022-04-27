#include "Texture.h"
#include <utility>

using namespace DirectX;

void Texture::LoadTexture(const wchar_t* szFile, HRESULT result,ID3D12Device* device)
{
	result = LoadFromWICFile(
		L"Resources/texture.png",			//Data\Resources\texture.pngを指定
		WIC_FLAGS_NONE, &metadata, scratchImg);

	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	metadata.format = MakeSRGB(metadata.format);

	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	texresDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texresDesc.Format = metadata.format;
	texresDesc.Width = metadata.width;			//幅
	texresDesc.Height = (UINT)metadata.height;	//高さ
	texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	texresDesc.MipLevels = (UINT16)metadata.mipLevels;
	texresDesc.SampleDesc.Count = 1;

	result = device->CreateCommittedResource(		//GPUリソースの生成
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,		//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texbuff));

	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = texbuff->WriteToSubresource(
			(UINT)i,
			nullptr,				//全領域へコピー
			img->pixels,			//元データアドレス
			(UINT)img->rowPitch,	//1ラインサイズ
			(UINT)img->slicePitch	//1枚サイズ
		);
		assert(SUCCEEDED(result));
	}
}
