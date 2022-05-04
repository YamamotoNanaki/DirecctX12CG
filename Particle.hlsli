cbuffer ConstBufferDataTransform : register(b0)
{
	matrix mat;		//3Dïœä∑çsóÒ
};

struct VSOutput
{
	float4 pos : SV_POSITION;
};

struct GSOutput
{
	float4 svpos : POSITION;
	float2 uv :TEXCOORD;
};