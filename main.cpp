#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <wrl.h>
#include "DxWindow.h"
#include "Dx12.h"
#include "Key.h"
#include "Object.h"

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

#pragma comment(lib,"d3d12.lib") 
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#pragma region �萔�o�b�t�@�\����

struct ConstBufferDataMaterial
{
	XMFLOAT4 color;	//�F(RGBA)
};

#pragma endregion �萔�o�b�t�@�\����

#pragma region ���_�f�[�^�\����
struct Vertex
{
	XMFLOAT3 pos;			//XYZ���W
	XMFLOAT3 normal;		//�@���x�N�g��
	XMFLOAT2 uv;			//UV���W
};
#pragma endregion ���_�f�[�^�\����

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {

	//--- �E�C���h�E�̐������� -----------------------------------------------------//

// �E�B���h�E�̐����i�O�����j
#pragma region �E�B���h�E�̐����i�O�����j

	// �E�B���h�E�T�C�Y
	const int window_width = 1280;  // ����
	const int window_height = 720;  // �c��

	DxWindow* win = new DxWindow(window_width, window_height);

#pragma endregion �E�B���h�E�̐����i�O�����j

	//------------------------------------------------------------------------------//

	MSG msg{}; // ���b�Z�[�W

#pragma region �f�o�b�O
#ifdef _DEBUG
// �f�o�b�N���C���[���I����
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif
#pragma endregion �f�o�b�O


	HRESULT result;

	Dx12* dx12 = new Dx12(result, win->hwnd, window_width, window_height);

	Key* key = new Key(result, win->w.hInstance, win->hwnd);


#pragma region �`�揉����

	#pragma region �[�x�o�b�t�@�̃��\�[�X�ݒ�

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = window_width;			//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResDesc.Height = window_height;		//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//�f�v�X�X�e���V��

	//�[�x�n�p�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//�[�x�n�̃N���A��ݒ�
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;			//�[�x�l1(�ő�l)�ŃN���A
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;		//�[�x�l�t�H�[�}�b�g

#pragma endregion �[�x�o�b�t�@�̃��\�[�X�ݒ�

	//------------------------

	#pragma region �[�x�o�b�t�@�̐���

//���\�[�X�ݒ�
	ComPtr < ID3D12Resource> depthBuffer = nullptr;
	result = dx12->device->CreateCommittedResource(
		&depthHeapProp, D3D12_HEAP_FLAG_NONE, &depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,					//�[�x�l�������݂Ɏg�p
		&depthClearValue, IID_PPV_ARGS(&depthBuffer));

#pragma endregion �[�x�o�b�t�@�̐���

	//---------------------------

	#pragma region �f�X�N���v�^�q�[�v�̐���(�[�x)

//�[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;						//�[�x�r���[��1��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//�f�v�X�X�e���V���r���[
	ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;
	result = dx12->device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//�[�x�r���[�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dx12->device->CreateDepthStencilView(
		depthBuffer.Get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());

#pragma endregion �f�X�N���v�^�q�[�v�̐���(�[�x)

	//---------------------------

	#pragma region �萔�o�b�t�@�̐����p�̐ݒ�
//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//GPU���\�[�X�̐���
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;
	result = dx12->device->CreateCommittedResource(
		&cbHeapProp,	//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,		//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial)
	);

	const size_t kObjectConst = 50;

	Object3d object3ds[kObjectConst];
	for (int i = 0; i < _countof(object3ds); i++)
	{
		object3ds[i].Initialize(result,dx12->device.Get());

		if (i > 0)
		{
			object3ds[i].parent = &object3ds[i - 1];
			object3ds[i].scale = { 0.9f,0.9f,0.9f };
			object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
			object3ds[i].position = { 0.0f,0.0f,-8.0f };
		}
	}

#pragma endregion �萔�o�b�t�@�̐����p�̐ݒ�

	//-------------------------

	#pragma region �萔�o�b�t�@�Ƀf�[�^��]������
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);	//�}�b�s���O
	assert(SUCCEEDED(result));
	constMapMaterial->color = XMFLOAT4(1, 1, 1, 1);					//RGBA�Ŕ������̐�
	constBuffMaterial->Unmap(0, nullptr);							//�}�b�s���O����


#pragma endregion �萔�o�b�t�@�Ƀf�[�^��]������

	//----------------------

	#pragma region �r���[�s��

	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);
	XMFLOAT3 target(0, 0, 0);
	XMFLOAT3 up(0, 1, 0);
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

#pragma endregion �r���[�s��

	#pragma region �ˉe�s��

	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f), (float)window_width / window_height, 0.1f, 1000.0f);

#pragma endregion �ˉe�s��

	#pragma region ���[���h�s��

		#pragma region �X�P�[�����O


		#pragma endregion �X�P�[�����O

		#pragma region ��]


		#pragma endregion ��]

		#pragma region ���s�ړ�


		#pragma endregion ���s�ړ�

#pragma endregion ���[���h�s��

	//-------------------------

	#pragma region �摜�C���[�W�f�[�^�̍쐬

	//WIC�e�N�X�`���̃��[�h
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		L"Resources/texture.png",			//Data\Resources\texture.png���w��
		WIC_FLAGS_NONE, &metadata, scratchImg);

	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	metadata.format = MakeSRGB(metadata.format);

	//const Image* img = scratchImg.GetImage(0, 0, 0);	//���f�[�^���o

	//const size_t textureWidth = 256;
	//const size_t textureHeight = 256;
	//const size_t imageDataCount = textureWidth * textureHeight;

	//XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];

	//	for (int i = 0; i < imageDataCount; i++)
	//	{
	//		imageData[i].x = 1.0f;
	//		imageData[i].y = 0.0f;
	//		imageData[i].z = 0.0f;
	//		imageData[i].w = 1.0f;
	//	}
#pragma endregion �摜�C���[�W�f�[�^�̍쐬

	#pragma region �e�N�X�`���o�b�t�@�̐���

		D3D12_HEAP_PROPERTIES texHeapProp{};	//�e�N�X�`���q�[�v�ݒ�
		texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

		//D3D12_RESOURCE_DESC texresDesc{};		//�e�N�X�`���ݒ�
		//texresDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//2d�e�N�X�`���p
		//texresDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			//RGBA�t�H�[�}�b�g
		//texresDesc.Width = textureWidth;			//��
		//texresDesc.Height = textureHeight;			//����
		//texresDesc.DepthOrArraySize = 1;
		//texresDesc.MipLevels = 1;
		//texresDesc.SampleDesc.Count = 1;

		//�摜�ǂݍ���
		D3D12_RESOURCE_DESC texresDesc{};		//�e�N�X�`���ݒ�
		//texresDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
		texresDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texresDesc.Format = metadata.format;
		texresDesc.Width = metadata.width;			//��
		texresDesc.Height = (UINT)metadata.height;	//����
		texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
		texresDesc.MipLevels = (UINT16)metadata.mipLevels;
		texresDesc.SampleDesc.Count = 1;

		ComPtr<ID3D12Resource> texbuff = nullptr;
		result = dx12->device->CreateCommittedResource(		//GPU���\�[�X�̐���
			&texHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,		//�e�N�X�`���p�w��
			nullptr,
			IID_PPV_ARGS(&texbuff));

#pragma endregion �e�N�X�`���o�b�t�@�̐���

	#pragma region �e�N�X�`���o�b�t�@�ւ̃f�[�^�]��
		
		for (size_t i = 0; i < metadata.mipLevels; i++)
		{
			const Image* img = scratchImg.GetImage(i, 0, 0);
			//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
			result = texbuff->WriteToSubresource(
				(UINT)i,
				nullptr,				//�S�̈�փR�s�[
				img->pixels,			//���f�[�^�A�h���X
				//sizeof(XMFLOAT4) * textureWidth,
				//sizeof(XMFLOAT4) * imageDataCount
				(UINT)img->rowPitch,	//1���C���T�C�Y
				(UINT)img->slicePitch	//1���T�C�Y
			);
			assert(SUCCEEDED(result));
		}

		//���f�[�^����i�Y�ꂸ�ɉ���j
		//delete[] imageData;

#pragma endregion �e�N�X�`���o�b�t�@�ւ̃f�[�^�]��

	//-------------------------

	#pragma region �f�X�N���v�^�q�[�v����

		const size_t kMaxSRVCount = 2056;

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeapDesc.NumDescriptors = kMaxSRVCount;

		ID3D12DescriptorHeap* srvHeap = nullptr;
		result = dx12->device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
		assert(SUCCEEDED(result));

		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
#pragma endregion �f�X�N���v�^�q�[�v����

	//-------------------------

	#pragma region �f�X�N���v�^�����W�̐ݒ�

		D3D12_DESCRIPTOR_RANGE descRangeSRV{};														//�e�N�X�`���p
		descRangeSRV.NumDescriptors = 1;															//�e�N�X�`�����
		descRangeSRV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;									//��ʂ̓e�N�X�`��
		descRangeSRV.BaseShaderRegister = 0;														//0�ԃX���b�g����
		descRangeSRV.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

#pragma endregion �f�X�N���v�^�����W�̐ݒ�

	//----------------------------

	#pragma region ���[�g�p�����[�^�̐ݒ�

	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//�萔�p
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//���
	rootParams[0].Descriptor.ShaderRegister = 0;								//�f�X�N���v�^�����W
	rootParams[0].Descriptor.RegisterSpace = 0;									//�f�X�N���v�^�����W��
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//���ׂẴV�F�[�_�[���猩����
	//�e�N�X�`���p
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//���
	rootParams[1].DescriptorTable.pDescriptorRanges = &descRangeSRV;				//�f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//�f�X�N���v�^�����W��
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//���ׂẴV�F�[�_�[���猩����
	//�萔�o�b�t�@2
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//���
	rootParams[2].Descriptor.ShaderRegister = 1;								//�f�X�N���v�^�����W
	rootParams[2].Descriptor.RegisterSpace = 0;									//�f�X�N���v�^�����W��
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//���ׂẴV�F�[�_�[���猩����

#pragma endregion ���[�g�p�����[�^�̐ݒ�

	//-----------------------
	//���_
	#pragma region ���_�f�[�^�[
	//���_�f�[�^
	Vertex vertices[] = {
		// x   y   z        u    v
		//�O
		{{-5, -5, -5},{},{0.0f, 1.0f}},	//����
		{{-5, +5, -5},{},{0.0f, 0.0f}},	//����
		{{+5, -5, -5},{},{1.0f, 1.0f}},	//�E��
		{{+5, +5, -5},{},{1.0f, 0.0f}},	//�E��
		//��			
		{{+5, -5, +5},{},{1.0f, 1.0f}},	//�E��
		{{+5, +5, +5},{},{1.0f, 0.0f}},	//�E��
		{{-5, -5, +5},{},{0.0f, 1.0f}},	//����
		{{-5, +5, +5},{},{0.0f, 0.0f}},	//����
		//��			
		{{-5, -5, -5},{},{0.0f, 1.0f}},	//����
		{{-5, -5, +5},{},{0.0f, 0.0f}},	//����
		{{-5, +5, -5},{},{1.0f, 1.0f}},	//�E��
		{{-5, +5, +5},{},{1.0f, 0.0f}},	//�E��
		//�E			
		{{+5, +5, -5},{},{1.0f, 1.0f}},	//�E��
		{{+5, +5, +5},{},{1.0f, 0.0f}},	//�E��
		{{+5, -5, -5},{},{0.0f, 1.0f}},	//����
		{{+5, -5, +5},{},{0.0f, 0.0f}},	//����
		//��			
		{{-5, +5, +5},{},{1.0f, 1.0f}},	//�E��
		{{+5, +5, +5},{},{1.0f, 0.0f}},	//�E��
		{{-5, +5, -5},{},{0.0f, 1.0f}},	//����
		{{+5, +5, -5},{},{0.0f, 0.0f}},	//����
		//��			
		{{-5, -5, -5},{},{0.0f, 1.0f}},	//����
		{{+5, -5, -5},{},{0.0f, 0.0f}},	//����
		{{-5, -5, +5},{},{1.0f, 1.0f}},	//�E��
		{{+5, -5, +5},{},{1.0f, 0.0f}},	//�E��
	};

	//�C���f�b�N�X�f�[�^
	unsigned short indices[] = {
		//
		0,1,2,
		2,1,3,
		//
		4,5,6,
		6,5,7,
		//
		8,9,10,
		10,9,11,
		//
		12,13,14,
		14,13,15,
		//
		16,17,18,
		18,17,19,
		//
		20,21,22,
		22,21,23
	};

	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));


#pragma endregion ���_�f�[�^�[

	//---------------------------

	#pragma region �@���x�N�g���̌v�Z

	for (int i = 0; i < _countof(indices) / 3; i++)
	{
		//
		//
		unsigned short index0 = indices[i * 3 + 0];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];
		//
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		//
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//
		normal = XMVector3Normalize(normal);
		//
		XMStoreFloat3(&vertices[index0].normal, normal);
		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
	}

#pragma endregion �@���x�N�g���̌v�Z

	//---------------------------

	#pragma region ���_�o�b�t�@�̊m��
	// ���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};   // �q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p

	D3D12_RESOURCE_DESC resDesc{};  // ���\�[�X�ݒ�
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // ���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ���_�o�b�t�@�̐���
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = dx12->device->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE, &resDesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

#pragma endregion ���_�o�b�t�@�̊m��

	//------------------------

	#pragma region �V�F�[�_�[���\�[�X�r���[

	//�V�F�[�_���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};			//�ݒ�\����
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	//RGBA
	srvDesc.Format = resDesc.Format;					//�摜�ǂݍ���
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;		//2d�e�N�X�`��
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	//�q�[�v�̂Q�ԖڂɃV�F�[�_�[���\�[�X�r���[�쐬
	dx12->device->CreateShaderResourceView(
		texbuff.Get(),		//�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc,		//�e�N�X�`���ݒ���
		srvHandle);

#pragma endregion �V�F�[�_�[���\�[�X�r���[

	//---------------------------

	#pragma region ���_�o�b�t�@�ւ̃f�[�^�]��
// GPU��̃o�b�t�@�ɑΉ��������z���������擾
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	// �S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];   // ���W���R�s�[
	}

	// �}�b�v������
	vertBuff->Unmap(0, nullptr);

#pragma endregion ���_�o�b�t�@�ւ̃f�[�^�]��

	//-----------------------------

	#pragma region ���_�o�b�t�@�r���[�̍쐬
	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

#pragma endregion ���_�o�b�t�@�փr���[�̍쐬

	//-------------------------------

	#pragma region �C���f�b�N�X�o�b�t�@�̐���

// �C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));
	// �C���f�b�N�X�o�b�t�@�̐ݒ�
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;//�C���f�b�N�X�o�b�t�@�̐���
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	result = dx12->device->CreateCommittedResource(
		&heapProp,				//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc,				//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

#pragma endregion �C���f�b�N�X�o�b�t�@�̐���

	//----------------------------

	#pragma region �C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	//�S�C���f�b�N�X�ɑ΂���
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];	//�C���f�b�N�X���R�s�[
	}

	//�Ȃ��������
	indexBuff->Unmap(0, nullptr);


	D3D12_INDEX_BUFFER_VIEW ibView{};

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

#pragma endregion �C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��

	//------------------------------

	#pragma region ���_�V�F�[�_�[�t�@�C���̓ǂݍ��݂ƃR���p�C��

	ComPtr<ID3DBlob> vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob = nullptr; // �G���[�I�u�W�F�N�g

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile( L"BasicVS.hlsl",  // �V�F�[�_�t�@�C����
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0, &vsBlob, &errorBlob);


#pragma endregion ���_�V�F�[�_�[�t�@�C���̓ǂݍ��݂ƃR���p�C��

	//-------------------------

	#pragma region �V�F�[�_�[�̃G���[���e��\��
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

#pragma endregion �V�F�[�_�[�̃G���[���e��\��

	//---------------------

	#pragma region �s�N�Z���V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile( L"BasicPS.hlsl",   // �V�F�[�_�t�@�C����
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0, &psBlob, &errorBlob);


#pragma endregion �s�N�Z���V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��

	//------------------------

	#pragma region �V�F�[�_�[�̃G���[���e��\��
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

#pragma endregion �V�F�[�_�[�̃G���[���e��\��

	//---------------------

	#pragma region ���_���C�A�E�g
	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{// xyz���W
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{// �@���x�N�g��
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{// uv���W
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

#pragma endregion ���_���C�A�E�g

	//------------------------

	#pragma region �O���t�B�b�N�X�p�C�v���C���ݒ�
// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

		#pragma region �f�v�X�X�e���V���X�e�[�g�̐ݒ�

	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	pipelineDesc.DepthStencilState.DepthEnable = true;		//�[�x�e�X�g���s��
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//�������݋���
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;		//�[�x�l�t�H�[�}�b�g

		#pragma endregion �f�v�X�X�e���V���X�e�[�g�̐ݒ�

	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;  // �w�ʂ��J�����O
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�
	pipelineDesc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	//�u�����h�ݒ�
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA�S�Ẵ`�����l����`��
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	blenddesc.BlendEnable = true;						//�u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;		//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;			//�\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;		//�f�X�g�̒l��  0%�g��

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//�\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//1.0f-�\�[�X�̃A���t�@�l

	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	pipelineDesc.NumRenderTargets = 1; // �`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

#pragma endregion �O���t�B�b�N�X�p�C�v���C���ݒ�

	//-----------------

	#pragma region �e�N�X�`���T���v���[�̐ݒ�

	D3D12_STATIC_SAMPLER_DESC samplerDesc{};

	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//���J��Ԃ�
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//�c�J��Ԃ�
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//���s�J��Ԃ�
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//���j�A�⊮
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									//�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;												//�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			//�s�N�Z���V�F�[�_�[����̂݉�


#pragma endregion �e�N�X�`���T���v���[�̐ݒ�

	//---------------------

	#pragma region ���[�g�V�O�l�`���̐ݒ�
	ComPtr<ID3D12RootSignature> rootsignature;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;
	rootSignatureDesc.NumParameters = _countof(rootParams);
	//�e�N�X�`���ǉ�
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dx12->device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();

	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootsignature.Get();
#pragma endregion ���[�g�V�O�l�`���̐ݒ�

	//----------------------

	#pragma region �p�C�v���C���X�e�[�g�̐���
	ComPtr<ID3D12PipelineState> pipelinestate = nullptr;
	result = dx12->device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelinestate));
	assert(SUCCEEDED(result));
#pragma endregion �p�C�v���C���X�e�[�g�̐���

#pragma endregion �`�揉����

	//�ϐ��錾
	//-----------------------

	float angleY = 0.0f;
	float angleX = 0.0f;
	//-----------------------



	while (!key->Down(key->ESC))
	{
#pragma region ���b�Z�[�W
		// ���b�Z�[�W������H
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); // �L�[���̓��b�Z�[�W�̏���
			DispatchMessage(&msg);  //�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
		}

		//X�{�^���ŏI�����b�Z�[�W��������Q�[�����[�v�𔲂���
		if (msg.message == WM_QUIT)
		{
			break;
		}

#pragma endregion ���b�Z�[�W

#pragma region DirectInput

		key->Update(result);

#pragma endregion DirectInput


		if (key->Judge(key->WASD, key->OR))
		{
			if (key->Down(key->D))angleY += XMConvertToRadians(1.0f);
			if (key->Down(key->A))angleY -= XMConvertToRadians(1.0f);
			if (key->Down(key->S))angleX += XMConvertToRadians(1.0f);
			if (key->Down(key->W))angleX -= XMConvertToRadians(1.0f);

			eye.x = -100 * sinf(angleY);
			eye.y = -100 * sinf(angleX);
			eye.z = -100 * cosf(angleY + angleX);

		matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
		}

		if (key->Judge(key->Arrow, key->OR))
		{
			for (int i = 0; i < 4; i++)
			{
				//�E
				if (key->Down(key->RIGHT))
				{
					object3ds[i].position.x += 1.0f;
				}
				//��
				if (key->Down(key->LEFT))
				{
					object3ds[i].position.x -= 1.0f;
				}
				//��
				if (key->Down(key->UP))
				{
					object3ds[i].position.y += 1.0f;
				}
				//��
				if (key->Down(key->DOWN))
				{
					object3ds[i].position.y -= 1.0f;
				}
			}
		}

		for (int i = 0; i < _countof(object3ds); i++)
		{
			object3ds[i].Update(matView, matProjection);
		}



#pragma region DirectX���t���[���̏��� �O

	#pragma region ���\�[�X�o���A�̕ύX
		//�o�b�N�o�b�t�@�̔ԍ����擾�i2�Ȃ̂�0�Ԃ�1�ԁj
		UINT bbIndex = dx12->swapchain->GetCurrentBackBufferIndex();

		// 1�D���\�[�X�o���A�ŏ������݉\�ɕύX
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = dx12->backBuffers[bbIndex].Get(); // �o�b�N�o�b�t�@���w��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // �\������
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`��
		dx12->commandList->ResourceBarrier(1, &barrierDesc);
#pragma endregion ���\�[�X�o���A�̕ύX

	#pragma region �����_�[�^�[�Q�b�g�r���[
		// 2�D�`���w��
		// �����_�[�^�[�Q�b�g�r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = dx12->rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * dx12->device->GetDescriptorHandleIncrementSize(dx12->rtvHeapDesc.Type);
		//�[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		dx12->commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvH);

#pragma endregion �����_�[�^�[�Q�b�g�r���[

	#pragma region ��ʃN���A
		// 3�D��ʃN���A           R     G     B    A
		float clearColor[4] = {0.1f ,0.25f ,0.5f ,1.0f};

		dx12->commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		dx12->commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
#pragma endregion ��ʃN���A

#pragma endregion DirectX���t���[���̏��� �O

	#pragma region �r���[�|�[�g�ݒ�R�}���h
		D3D12_VIEWPORT viewport{};

		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		dx12->commandList->RSSetViewports(1, &viewport);
#pragma endregion �r���[�|�[�g�ݒ�R�}���h5

	#pragma region �V�U�[��`�̐ݒ�R�}���h
		D3D12_RECT scissorrect{};

		scissorrect.left = 0;
		scissorrect.right = scissorrect.left + window_width;
		scissorrect.top = 0;
		scissorrect.bottom = scissorrect.top + window_height;

		dx12->commandList->RSSetScissorRects(1, &scissorrect);
#pragma endregion �V�U�[��`�̐ݒ�R�}���h

	#pragma region �p�C�v���C���X�e�[�g
		dx12->commandList->SetPipelineState(pipelinestate.Get());
		dx12->commandList->SetGraphicsRootSignature(rootsignature.Get());

		////�f�X�N���v�^�q�[�v��ݒ�
		//ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap };
		//cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		////�萔�o�b�t�@�r���[���Z�b�g
		////cmdList->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());

		////�e�N�X�`���ǉ�
		////�f�X�N���v�^�q�[�v�̐擪�n���h�����擾(GPU��)
		//D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		////�q�[�v�̐擪�ɂ���CBV�����[�g�p�����[�^0�Ԃɐݒ�
		//cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandle);
		////�n���h����1�i�߂�
		//gpuDescHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		////�q�[�v��2�Ԗڂɂ���SRV�����[�g�p�����[�^��1�Ԃɐݒ�
		//cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandle);

#pragma endregion �p�C�v���C���X�e�[�g

	#pragma region �v���~�e�B�u�`��
		dx12->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#pragma endregion �v���~�e�B�u�`��

	#pragma region ���_�o�b�t�@�r���[�̐ݒ�R�}���h
		dx12->commandList->IASetVertexBuffers(0, 1, &vbView);
#pragma endregion ���_�o�b�t�@�r���[�̐ݒ�R�}���h

	#pragma region �C���f�b�N�X�o�b�t�@�r���[�̃Z�b�g
		dx12->commandList->IASetIndexBuffer(&ibView);
#pragma endregion �C���f�b�N�X�o�b�t�@�r���[�̃Z�b�g

	#pragma region �萔�o�b�t�@�r���[�̐ݒ�R�}���h

		dx12->commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
		dx12->commandList->SetDescriptorHeaps(1, &srvHeap);
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
		dx12->commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

#pragma endregion �萔�o�b�t�@�r���[�̐ݒ�R�}���h

	#pragma region �`��R�}���h
		for (int i = 0; i < _countof(object3ds); i++)
		{
			object3ds[i].Draw(dx12->commandList.Get(), vbView, ibView, _countof(indices));
		}
#pragma endregion �`��R�}���h

#pragma region DirectX���t���[���̏��� ��

	#pragma region ���\�[�X�o���A�̕��A�R�}���h
// 5�D���\�[�X�o���A��߂�
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`��
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;   // �\����
		dx12->commandList->ResourceBarrier(1, &barrierDesc);
#pragma endregion ���\�[�X�o���A�̕��A�R�}���h

	#pragma region ���߂Ă��������߂����s����
		// ���߂̃N���[�Y
		result = dx12->commandList->Close();
		assert(SUCCEEDED(result));
		// �R�}���h���X�g�̎��s
		ID3D12CommandList* commandLists[] = { dx12->commandList.Get()}; // �R�}���h���X�g�̔z��
		dx12->commandQueue->ExecuteCommandLists(1, commandLists);

		// �o�b�t�@���t���b�v�i���\�̓��ւ��j
		result = dx12->swapchain->Present(1, 0);
		assert(SUCCEEDED(result));
#pragma endregion ���߂Ă��������߂����s����

	#pragma region ��ʓ���ւ�
		// �R�}���h���X�g�̎��s������҂�
		dx12->commandQueue->Signal(dx12->fence.Get(), ++dx12->fenceVal);

		if (dx12->fence->GetCompletedValue() != dx12->fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			dx12->fence->SetEventOnCompletion(dx12->fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		result = dx12->commandAllocator->Reset(); // �L���[���N���A
		assert(SUCCEEDED(result));
		result = dx12->commandList->Reset(dx12->commandAllocator.Get(), nullptr);  // �ĂуR�}���h���X�g�𒙂߂鏀��
		assert(SUCCEEDED(result));
#pragma endregion ��ʓ���ւ�

#pragma endregion DirectX���t���[���̏��� ��
	}

	return 0;
}