#pragma once
#include <d3d12.h>

class GPipeline
{
public:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc[4] = {};
	GPipeline(ID3DBlob* vsBlob, ID3DBlob* psBlob, D3D12_INPUT_ELEMENT_DESC* inputLayout, int layoutCount);
	void RootSignature(ID3D12RootSignature& rootsignature);
};