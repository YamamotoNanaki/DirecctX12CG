cbuffer cbuff0:register(b0)
{
	float4 color;	//êF(RGBA)
};

cbuffer ConstBufferDataTransform : register(b1)
{
	matrix mat;		//3Dïœä∑çsóÒ
	matrix matBillboard;
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