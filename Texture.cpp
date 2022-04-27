#include "Texture.h"
#include <utility>

using namespace DirectX;

void Texture::LoadTexture(const wchar_t* szFile, HRESULT result,ID3D12Device* device)
{
	result = LoadFromWICFile(
		L"Resources/texture.png",			//Data\Resources\texture.png���w��
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
	texresDesc.Width = metadata.width;			//��
	texresDesc.Height = (UINT)metadata.height;	//����
	texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	texresDesc.MipLevels = (UINT16)metadata.mipLevels;
	texresDesc.SampleDesc.Count = 1;

	result = device->CreateCommittedResource(		//GPU���\�[�X�̐���
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,		//�e�N�X�`���p�w��
		nullptr,
		IID_PPV_ARGS(&texbuff));

	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texbuff->WriteToSubresource(
			(UINT)i,
			nullptr,				//�S�̈�փR�s�[
			img->pixels,			//���f�[�^�A�h���X
			(UINT)img->rowPitch,	//1���C���T�C�Y
			(UINT)img->slicePitch	//1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}
}