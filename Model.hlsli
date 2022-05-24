cbuffer cbuff0:register(b0)
{
	float4 color;	//�F(RGBA)
};

cbuffer ConstBufferDataTransform : register(b1)
{
	matrix viewPro;		//3D�ϊ��s��
	matrix world;
	float3 cameraPos;
};

cbuffer Material : register(b2)
{
	float3 ambient :packoffset(c0);
	float3 diffuse : packoffset(c1);
	float3 specular : packoffset(c2);
	float alpha : packoffset(c2.w);
};

cbuffer LightBuff : register(b3)
{
	float3 lightv;
	float3 lightcolor;
};

struct VSOutput
{
	float4 svpos : SV_POSITION;
	float4 color :COLOR;
	float2 uv :TEXCOORD;
};

struct GSOutput
{
	float4 svpos : SV_POSITION;
	float4 color :COLOR;
	float2 uv :TEXCOORD;
};