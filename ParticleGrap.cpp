#include "ParticleGrap.h"
#include <cassert>
#include <string>
#include <d3dcompiler.h>

using namespace std;
using namespace IF;

HRESULT ParticleGrap::CompillerVS(LPCWSTR fillname)
{
	HRESULT result;

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(fillname,  // シェーダファイル名
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル	指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0, &vsBlob, &errorBlob);

	//-------------------------

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	return result;
}

HRESULT ParticleGrap::CompillerPS(LPCWSTR fillname)
{
	HRESULT result;

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(fillname,  // シェーダファイル名
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル	指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0, &psBlob, &errorBlob);

	//-------------------------

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	return result;
}

HRESULT ParticleGrap::CompillerGS(LPCWSTR fillname)
{
	HRESULT result;

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(fillname,  // シェーダファイル名
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "gs_5_0", // エントリーポイント名、シェーダーモデル	指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0, &gsBlob, &errorBlob);

	//-------------------------

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	return result;
}

HRESULT ParticleGrap::Compiller(LPCWSTR vs, LPCWSTR ps, LPCWSTR gs)
{
	HRESULT result;
	result = CompillerVS(vs);
	result = CompillerPS(ps);
	result = CompillerGS(gs);

	return result;
}

HRESULT ParticleGrap::Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_RANGE& descRangeSRV)
{
	HRESULT result;

	RootParam root(descRangeSRV, 1);

	result = Compiller(L"ParticleVS.hlsl", L"ParticlePS.hlsl", L"ParticleGS.hlsl");

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{// xyz座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	GPipeline pipeline(vsBlob.Get(), psBlob.Get(), gsBlob.Get(), inputLayout, _countof(inputLayout));

	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//横繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//縦繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//奥行繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//リニア補完
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									//ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;												//ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			//ピクセルシェーダーからのみ可視

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &root.rootParams.front();
	rootSignatureDesc.NumParameters = root.rootParams.size();
	//テクスチャ追加
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();

	pipeline.RootSignature(*rootsignature.Get());

	for (int i = 0; i < _countof(pipelinestate); i++)
	{
		int j = i;
		if (j > 3)j = 0;
		result = device->CreateGraphicsPipelineState(&pipeline.pipelineDesc[j], IID_PPV_ARGS(&pipelinestate[j]));
	}
	assert(SUCCEEDED(result));

	return result;
}

void IF::ParticleGrap::DrawBlendMode(ID3D12GraphicsCommandList* commandList, Blend blend)
{
	commandList->SetPipelineState(pipelinestate[blend].Get());
}
