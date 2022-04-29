#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <wrl.h>
#include "Window.h"
#include "DirectX12.h"
#include "Key.h"
#include "Object.h"
#include "Projection.h"
#include "View.h"
#include "Texture.h"
#include "RootParam.h"
#include "GPipeline.h"

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

#pragma comment(lib,"d3d12.lib") 
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
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
	const int winWidth = 1280;  // ����
	const int winHeight = 720;  // �c��

	Window* win = new Window(winWidth, winHeight);

#pragma endregion �E�B���h�E�̐����i�O�����j

	//------------------------------------------------------------------------------//


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
	DirectX12* dx12 = new DirectX12(result, win->hwnd, winWidth, winHeight);
	Key* key = new Key(result, win->w.hInstance, win->hwnd);

#pragma region �`�揉����

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

	Object object3ds[kObjectConst];
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

	View matView;

#pragma endregion �r���[�s��

	#pragma region �ˉe�s��

	Projection matPro(45.0f, winWidth, winHeight);

#pragma endregion �ˉe�s��

	Texture tex;

	tex.LoadTexture(L"Resources/texture.png", dx12->device.Get());

	#pragma region ���[�g�p�����[�^�̐ݒ�

	RootParam root(tex.descRangeSRV, 1);

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
		tex.texbuff.Get(),		//�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc,		//�e�N�X�`���ݒ���
		tex.srvHandle);

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

	GPipeline gPipeline(vsBlob.Get(), psBlob.Get(), inputLayout, _countof(inputLayout));


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
	rootSignatureDesc.pParameters = &root.rootParams.front();
	rootSignatureDesc.NumParameters = root.rootParams.size();
	//�e�N�X�`���ǉ�
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dx12->device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();

	gPipeline.RootSignature(*rootsignature.Get());
#pragma endregion ���[�g�V�O�l�`���̐ݒ�

	//----------------------

	#pragma region �p�C�v���C���X�e�[�g�̐���
	ComPtr<ID3D12PipelineState> pipelinestate = nullptr;
	result = dx12->device->CreateGraphicsPipelineState(&gPipeline.pipelineDesc[0], IID_PPV_ARGS(&pipelinestate));
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
		//���b�Z�[�W
		if (win->Message())break;
		//�L�[�A�b�v�f�[�g
		key->Update(result);


		if (key->Judge(key->WASD, key->OR))
		{
			if (key->Down(key->D))angleY += XMConvertToRadians(1.0f);
			if (key->Down(key->A))angleY -= XMConvertToRadians(1.0f);
			if (key->Down(key->S))angleX += XMConvertToRadians(1.0f);
			if (key->Down(key->W))angleX -= XMConvertToRadians(1.0f);

			matView.eye.x = -100 * sinf(angleY);
			matView.eye.y = -100 * sinf(angleX);
			matView.eye.z = -100 * cosf(angleY + angleX);

			matView.Update();
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
			object3ds[i].Update(matView.Get(), matPro.Get());
		}


		dx12->DrawBefore();

	#pragma region �r���[�|�[�g�ݒ�R�}���h
		D3D12_VIEWPORT viewport{};

		viewport.Width = winWidth;
		viewport.Height = winHeight;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		dx12->commandList->RSSetViewports(1, &viewport);
#pragma endregion �r���[�|�[�g�ݒ�R�}���h5

	#pragma region �V�U�[��`�̐ݒ�R�}���h
		D3D12_RECT scissorrect{};

		scissorrect.left = 0;
		scissorrect.right = scissorrect.left + winWidth;
		scissorrect.top = 0;
		scissorrect.bottom = scissorrect.top + winHeight;

		dx12->commandList->RSSetScissorRects(1, &scissorrect);
#pragma endregion �V�U�[��`�̐ݒ�R�}���h

	#pragma region �p�C�v���C���X�e�[�g
		dx12->commandList->SetPipelineState(pipelinestate.Get());
		dx12->commandList->SetGraphicsRootSignature(rootsignature.Get());
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
		dx12->commandList->SetDescriptorHeaps(1, &tex.srvHeap);
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = tex.srvHeap->GetGPUDescriptorHandleForHeapStart();
		dx12->commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

#pragma endregion �萔�o�b�t�@�r���[�̐ݒ�R�}���h

	#pragma region �`��R�}���h
		for (int i = 0; i < _countof(object3ds); i++)
		{
			object3ds[i].Draw(dx12->commandList.Get(), vbView, ibView, _countof(indices));
		}
#pragma endregion �`��R�}���h

		dx12->DrawAfter(result);
	}

	return 0;
}