#include <DirectXMath.h>
#include "Window.h"
#include "DirectX12.h"
#include "Key.h"
#include "Object.h"
#include "Projection.h"
#include "View.h"
#include "Texture.h"
#include "RootParam.h"
#include "GPipeline.h"
#include "Graphic.h"
#include "VertexIndex.h"
#include "ConstBuff.h"

using namespace DirectX;
using namespace IF;


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
	ConstBuff cb;
	result = cb.Initialize(dx12->device.Get());

	const size_t kObjectConst = 50;

	Object object3ds[kObjectConst];
	for (int i = 0; i < _countof(object3ds); i++)
	{
		result = object3ds[i].Initialize(dx12->device.Get());

		if (i > 0)
		{
			object3ds[i].parent = &object3ds[i - 1];
			object3ds[i].scale = { 0.9f,0.9f,0.9f };
			object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
			object3ds[i].position = { 0.0f,0.0f,-8.0f };
		}
	}

	#pragma region �r���[�s��

	View matView;

#pragma endregion �r���[�s��

	#pragma region �ˉe�s��

	Projection matPro(45.0f, winWidth, winHeight);

#pragma endregion �ˉe�s��

	Texture tex;

	tex.LoadTexture(L"Resources/texture.png", dx12->device.Get());

	for (int i = 0; i < _countof(object3ds); i++)
	{
		object3ds[i].VIInitialize(dx12->device.Get(), tex.texbuff.Get(), tex.srvHandle);
	}

	Graphic graph;
	result = graph.Initialize(dx12->device.Get(), tex.descRangeSRV);

#pragma endregion �`�揉����

	//�ϐ��錾
	//-----------------------

	float angleY = 0.0f;
	float angleX = 0.0f;
	float color = 0;
	//-----------------------

	while (!key->Down(key->ESC))
	{
		//���b�Z�[�W
		if (win->Message())break;
		//�L�[�A�b�v�f�[�g
		key->Update();


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

		color+=0.005;
		if (color >= 1.0f)
		{
			color = 0;
		}
		//result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);	//�}�b�s���O
		//assert(SUCCEEDED(result));
		//constMapMaterial->color = XMFLOAT4(color, 0, 0, 1);					//RGBA�Ŕ������̐�
		//constBuffMaterial->Unmap(0, nullptr);							//�}�b�s���O����


		dx12->DrawBefore();

	#pragma region �V�U�[��`�̐ݒ�R�}���h
		D3D12_RECT scissorrect{};

		scissorrect.left = 0;
		scissorrect.right = scissorrect.left + winWidth;
		scissorrect.top = 0;
		scissorrect.bottom = scissorrect.top + winHeight;

		dx12->commandList->RSSetScissorRects(1, &scissorrect);
#pragma endregion �V�U�[��`�̐ݒ�R�}���h

	#pragma region �p�C�v���C���X�e�[�g
		dx12->commandList->SetGraphicsRootSignature(graph.rootsignature.Get());
#pragma endregion �p�C�v���C���X�e�[�g

	#pragma region �v���~�e�B�u�`��
		dx12->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#pragma endregion �v���~�e�B�u�`��

		dx12->commandList->SetGraphicsRootConstantBufferView(0, cb.GetGPUAddress());
		dx12->commandList->SetDescriptorHeaps(1, &tex.srvHeap);
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = tex.srvHeap->GetGPUDescriptorHandleForHeapStart();
		dx12->commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

			dx12->commandList->SetPipelineState(graph.pipelinestate[0].Get());

	#pragma region �`��R�}���h
		for (int i = 0; i < _countof(object3ds); i++)
		{
			object3ds[i].Draw(dx12->commandList.Get(),dx12->viewport);
		}
#pragma endregion �`��R�}���h

		dx12->DrawAfter();
	}

	delete key;
	delete dx12;
	delete win;

	return 0;
}