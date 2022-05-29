#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include "GPipeline.h"
#include "RootParam.h"

#pragma comment(lib,"d3dcompiler.lib")

namespace IF
{
	namespace Blend
	{
		enum Blend
		{
			NORMAL,	  //0
			ADD,	  //1
			SUB,	  //2
			INVSRC,	  //3
		};
	}
	namespace ShaderCode
	{
		enum ShaderCode
		{
			error,	  //0
			vs,		  //1
			ps,		  //2
			gs,		  //3
		};
	}
	namespace ShaderCompile
	{
		enum ShaderCompile
		{
			vs = 0b001,
			ps = 0b010,
			gs = 0b100,
			vsps = 0b011,
			all = 0b111,
		};
	}

	class Graphic
	{
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	private:
		std::vector<ComPtr<ID3DBlob>>Blobs;

	public:
		ComPtr<ID3D12RootSignature> rootsignature;
		ComPtr<ID3D12PipelineState> pipelinestate[4]{ nullptr };

	public:
		HRESULT CompillerArray(LPCWSTR fillname, int num);
		HRESULT Compiller(LPCWSTR vs, LPCWSTR ps, LPCWSTR gs, char Compile = ShaderCompile::all);
		HRESULT Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_RANGE& descRangeSRV);
		HRESULT Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_RANGE& descRangeSRV, LPCWSTR vs, LPCWSTR ps, LPCWSTR gs);
		HRESULT Initialize2D(ID3D12Device* device, D3D12_DESCRIPTOR_RANGE& descRangeSRV, LPCWSTR vs, LPCWSTR ps);
		HRESULT ParticleInitialize(ID3D12Device* device, D3D12_DESCRIPTOR_RANGE& descRangeSRV);
		void DrawBlendMode(ID3D12GraphicsCommandList* commandList, Blend::Blend blend = Blend::NORMAL);
	};
}