#include "GPipeline.h"

GPipeline::GPipeline(ID3DBlob& vs, ID3DBlob& ps, D3D12_INPUT_ELEMENT_DESC* inputLayout)
{
	for (int i = 0; i < _countof(pipelineDesc); i++)
	{
		pipelineDesc[i].VS.pShaderBytecode = vs.GetBufferPointer();
		pipelineDesc[i].VS.BytecodeLength = vs.GetBufferSize();
		pipelineDesc[i].PS.pShaderBytecode = ps.GetBufferPointer();
		pipelineDesc[i].PS.BytecodeLength = ps.GetBufferSize();

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		pipelineDesc[i].DepthStencilState.DepthEnable = true;		//�[�x�e�X�g���s��
		pipelineDesc[i].DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//�������݋���
		pipelineDesc[i].DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		pipelineDesc[i].DSVFormat = DXGI_FORMAT_D32_FLOAT;		//�[�x�l�t�H�[�}�b�g

#pragma endregion �f�v�X�X�e���V���X�e�[�g�̐ݒ�

		pipelineDesc[i].SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
		pipelineDesc[i].RasterizerState.CullMode = D3D12_CULL_MODE_BACK;  // �w�ʂ��J�����O
		pipelineDesc[i].RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�
		pipelineDesc[i].RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

		pipelineDesc[i].InputLayout.pInputElementDescs = inputLayout;
		pipelineDesc[i].InputLayout.NumElements = sizeof(inputLayout) / sizeof(inputLayout[0]);
				
		pipelineDesc[i].PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				
		pipelineDesc[i].NumRenderTargets = 1; // �`��Ώۂ�1��
		pipelineDesc[i].RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA
		pipelineDesc[i].SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O
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
		// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
		pipelineDesc[i].pRootSignature = &rootsignature;
	}
}
