cbuffer ConstBufferDataTransform : register(b0)
{
	matrix mat;		//3D�ϊ��s��
};

struct VSOutput
{
	float4 pos : SV_POSITION;
};

struct GSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv :TEXCOORD;
};