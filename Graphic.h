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
		ComPtr<ID3DBlob> errorBlob = nullptr;		// エラーオブジェクト
		ComPtr<ID3DBlob> vsBlob = nullptr;			// 頂点シェーダオブジェクト
		ComPtr<ID3DBlob> psBlob = nullptr;			// ピクセルシェーダオブジェクト
		ComPtr<ID3DBlob> gsBlob = nullptr;			// ジオメトリシェーダオブジェクト
		
	public:
		ComPtr<ID3D12RootSignature> rootsignature;
		ComPtr<ID3D12PipelineState> pipelinestate[4]{ nullptr };

	public:
		HRESULT CompillerVS(LPCWSTR fillname);
		HRESULT CompillerPS(LPCWSTR fillname);
		HRESULT CompillerGS(LPCWSTR fillname);
		HRESULT Compiller(LPCWSTR vs, LPCWSTR ps, LPCWSTR gs);
		HRESULT Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_RANGE& descRangeSRV, int cbnum);
		HRESULT Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_RANGE& descRangeSRV, int cbnum, LPCWSTR vs, LPCWSTR ps, LPCWSTR gs);
		HRESULT InitializeParticle(ID3D12Device* device, D3D12_DESCRIPTOR_RANGE& descRangeSRV);
		void BlendMode(ID3D12GraphicsCommandList* commandList, Blend blend = NORMAL);
	};
}