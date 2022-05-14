#include "Texture.h"
#include <utility>

using namespace DirectX;
using namespace IF;

HRESULT Texture::TexLoad(const wchar_t* szFile)
{
	HRESULT result = LoadFromWICFile(
		szFile,			//Data\Resources\texture.pngを指定
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
	return result;
}

HRESULT Texture::LoadBuffer()
{
	D3D12_HEAP_PROPERTIES texHeapProp{};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	D3D12_RESOURCE_DESC texresDesc{};
	texresDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texresDesc.Format = metadata.format;
	texresDesc.Width = metadata.width;			//幅
	texresDesc.Height = (UINT)metadata.height;	//高さ
	texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	texresDesc.MipLevels = (UINT16)metadata.mipLevels;
	texresDesc.SampleDesc.Count = 1;

	HRESULT result = device->CreateCommittedResource(		//GPUリソースの生成
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,		//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texbuff));

	return result;
}

void Texture::LoadTransfer(HRESULT result)
{
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

HRESULT Texture::Heap()
{
	const size_t kMaxSRVCount = 2056;

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	HRESULT result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	return result;
}

void Texture::Range()
{
	descRangeSRV.NumDescriptors = 1;															//テクスチャ一つ
	descRangeSRV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;									//種別はテクスチャ
	descRangeSRV.BaseShaderRegister = 0;														//0番スロットから
	descRangeSRV.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

void IF::Texture::Initialize(ID3D12Device* device)
{
	this->device = device;
}


HRESULT Texture::Loadtexture(const wchar_t* szFile)
{
	HRESULT result = TexLoad(szFile);
	result = LoadBuffer();
	LoadTransfer(result);
	result = Heap();
	Range();

	return result;
}
