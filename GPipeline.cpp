#include "GPipeline.h"

GPipeline::GPipeline(ID3DBlob& vs, ID3DBlob& ps, D3D12_INPUT_ELEMENT_DESC* inputLayout)
{
	for (int i = 0; i < _countof(pipelineDesc); i++)
	{
		pipelineDesc[i].VS.pShaderBytecode = vs.GetBufferPointer();
		pipelineDesc[i].VS.BytecodeLength = vs.GetBufferSize();
		pipelineDesc[i].PS.pShaderBytecode = ps.GetBufferPointer();
		pipelineDesc[i].PS.BytecodeLength = ps.GetBufferSize();

		//デプスステンシルステートの設定
		pipelineDesc[i].DepthStencilState.DepthEnable = true;		//深度テストを行う
		pipelineDesc[i].DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//書き込み許可
		pipelineDesc[i].DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		pipelineDesc[i].DSVFormat = DXGI_FORMAT_D32_FLOAT;		//深度値フォーマット

#pragma endregion デプスステンシルステートの設定

		pipelineDesc[i].SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
		pipelineDesc[i].RasterizerState.CullMode = D3D12_CULL_MODE_BACK;  // 背面をカリング
		pipelineDesc[i].RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
		pipelineDesc[i].RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

		pipelineDesc[i].InputLayout.pInputElementDescs = inputLayout;
		pipelineDesc[i].InputLayout.NumElements = sizeof(inputLayout) / sizeof(inputLayout[0]);
				
		pipelineDesc[i].PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				
		pipelineDesc[i].NumRenderTargets = 1; // 描画対象は1つ
		pipelineDesc[i].RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
		pipelineDesc[i].SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング
	}
	blendDesc = new Blend(pipelineDesc);
}

GPipeline::~GPipeline()
{
	delete blendDesc;
}

void GPipeline::RootSignature(ID3D12RootSignature& rootsignature)
{
	for (int i = 0; i < _countof(pipelineDesc); i++)
	{
		// パイプラインにルートシグネチャをセット
		pipelineDesc[i].pRootSignature = &rootsignature;
	}
}
