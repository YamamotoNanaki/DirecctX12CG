#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "GPipeline.h"
#include "RootParam.h"

#pragma comment(lib,"d3dcompiler.lib")

namespace IF
{
	enum Blend
	{
		NORMAL,	  //0
		ADD,	  //1
		SUB,	  //2
		INVSRC,	  //3
	};

	class Graphic
	{
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	private:
		ComPtr<ID3DBlob> errorBlob = nullptr;		// �G���[�I�u�W�F�N�g
		ComPtr<ID3DBlob> vsBlob = nullptr;			// ���_�V�F�[�_�I�u�W�F�N�g
		ComPtr<ID3DBlob> psBlob = nullptr;			// �s�N�Z���V�F�[�_�I�u�W�F�N�g
		ComPtr<ID3DBlob> gsBlob = nullptr;			// �W�I���g���V�F�[�_�I�u�W�F�N�g
		
	public:
		ComPtr<ID3D12RootSignature> rootsignature;
		ComPtr<ID3D12PipelineState> pipelinestate[4]{ nullptr };

	public:
		HRESULT CompillerVS();
		HRESULT CompillerPS();
		HRESULT CompillerGS();
		HRESULT Compiller();
		HRESULT Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_RANGE& descRangeSRV);
		void DrawBlendMode(ID3D12GraphicsCommandList* commandList, Blend blend = NORMAL);
	};
}