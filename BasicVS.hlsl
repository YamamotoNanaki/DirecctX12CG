//#include "Basic.hlsli"
//
//VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
//{
//	VSOutput output;//�s�N�Z���V�F�[�_�[�ɓn���l
//	//output.svpos = pos;			//���̃R�[�h
//	output.svpos = mul(mat, pos);	//���W�ɍs�����Z
//	output.uv = uv;
//	return output;
//}

float4 main(float4 pos : POSITION) : SV_POSITION
{
	return pos * float4( 1.5f, 0.5f, 1.0f, 1.0f ) + float4( -0.2f ,0.5f , 0, 0 );
}