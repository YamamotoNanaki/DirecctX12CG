#include "Blend.h"

Blend::Blend(D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelineDesc)
{
	int max = _countof(blenddesc);
	if (_countof(blenddesc) < sizeof(pipelineDesc) / sizeof(pipelineDesc[0]))
	{
		max = sizeof(pipelineDesc) / sizeof(pipelineDesc[0]);
	}
	for (int i = 0; i < max; i++)
	{
		blenddesc[i] = pipelineDesc[i].BlendState.RenderTarget[0];
		blenddesc[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
				  
		blenddesc[i].BlendEnable = true;						//ブレンドを有効にする
		blenddesc[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;			//加算
		blenddesc[i].SrcBlendAlpha = D3D12_BLEND_ONE;			//ソースの値を100%使う
		blenddesc[i].DestBlendAlpha = D3D12_BLEND_ZERO;			//デストの値を  0%使う

		if (i >= _countof(blenddesc))
		{
			blenddesc[i].BlendOp = D3D12_BLEND_OP_ADD;				//加算
			blenddesc[i].SrcBlend = D3D12_BLEND_SRC_ALPHA;			//ソースのアルファ値
			blenddesc[i].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;		//1.0f-ソースのアルファ値
		}
	}

	blenddesc[0].BlendOp = D3D12_BLEND_OP_ADD;				//加算
	blenddesc[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;			//ソースのアルファ値
	blenddesc[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;		//1.0f-ソースのアルファ値

	blenddesc[1].BlendOp = D3D12_BLEND_OP_ADD;				//加算
	blenddesc[1].SrcBlend = D3D12_BLEND_ONE;				//ソースの値を100%使う
	blenddesc[1].DestBlend = D3D12_BLEND_ONE;				//ソースの値を100%使う

	blenddesc[2].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;		//デストからソースを減算
	blenddesc[2].SrcBlend = D3D12_BLEND_ONE;				//ソースの値を100%使う
	blenddesc[2].DestBlend = D3D12_BLEND_ONE;				//ソースの値を100%使う

	blenddesc[3].BlendOp = D3D12_BLEND_OP_ADD;				//加算
	blenddesc[3].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;		//1.0f-デストカラーの値
	blenddesc[3].DestBlend = D3D12_BLEND_ZERO;				//使わない
}
