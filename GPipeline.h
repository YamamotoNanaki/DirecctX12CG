#pragma once
#include "Blend.h"

class GPipeline
{
private:
	Blend* blendDesc;

public:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc[4];
	GPipeline(D3D12_INPUT_ELEMENT_DESC* inputLayout, int layoutCount);
	~GPipeline();
	void RootSignature(ID3D12RootSignature& rootsignature);
};