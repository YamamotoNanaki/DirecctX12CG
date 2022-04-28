#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib") 

class Blend
{
private:
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc[4];

public:
	Blend(D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelineDesc, int PipelineCount);

};