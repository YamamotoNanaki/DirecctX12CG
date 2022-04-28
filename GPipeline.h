#pragma once
#include "Blend.h"

class GPipeline
{
private:
	Blend* blendDesc;

public:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc[4];
	GPipeline(ID3DBlob& vs, ID3DBlob& ps, D3D12_INPUT_ELEMENT_DESC* inputLayout);
	~GPipeline();
	void RootSignature(ID3D12RootSignature& rootsignature);
};