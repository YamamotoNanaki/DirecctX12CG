#include "Blend.h"

Blend::Blend(D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelineDesc, int PipelineCount)
{
	int max = _countof(blenddesc);
	if (max < PipelineCount)
	{
		max = PipelineCount;
	}
	for (int i = 0; i < max; i++)
	{
		blenddesc[i] = pipelineDesc[i].BlendState.RenderTarget[0];
		blenddesc[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
				  
		blenddesc[i].BlendEnable = true;						//�u�����h��L���ɂ���
		blenddesc[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;			//���Z
		blenddesc[i].SrcBlendAlpha = D3D12_BLEND_ONE;			//�\�[�X�̒l��100%�g��
		blenddesc[i].DestBlendAlpha = D3D12_BLEND_ZERO;			//�f�X�g�̒l��  0%�g��

		if (i >= _countof(blenddesc))
		{
			blenddesc[i].BlendOp = D3D12_BLEND_OP_ADD;				//���Z
			blenddesc[i].SrcBlend = D3D12_BLEND_SRC_ALPHA;			//�\�[�X�̃A���t�@�l
			blenddesc[i].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;		//1.0f-�\�[�X�̃A���t�@�l
		}
	}

	blenddesc[0].BlendOp = D3D12_BLEND_OP_ADD;				//���Z
	blenddesc[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;			//�\�[�X�̃A���t�@�l
	blenddesc[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;		//1.0f-�\�[�X�̃A���t�@�l

	blenddesc[1].BlendOp = D3D12_BLEND_OP_ADD;				//���Z
	blenddesc[1].SrcBlend = D3D12_BLEND_ONE;				//�\�[�X�̒l��100%�g��
	blenddesc[1].DestBlend = D3D12_BLEND_ONE;				//�\�[�X�̒l��100%�g��

	blenddesc[2].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;		//�f�X�g����\�[�X�����Z
	blenddesc[2].SrcBlend = D3D12_BLEND_ONE;				//�\�[�X�̒l��100%�g��
	blenddesc[2].DestBlend = D3D12_BLEND_ONE;				//�\�[�X�̒l��100%�g��

	blenddesc[3].BlendOp = D3D12_BLEND_OP_ADD;				//���Z
	blenddesc[3].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;		//1.0f-�f�X�g�J���[�̒l
	blenddesc[3].DestBlend = D3D12_BLEND_ZERO;				//�g��Ȃ�
}
