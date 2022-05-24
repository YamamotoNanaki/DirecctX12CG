#include "Model.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	float3 lightdir = float3(1, -1, 1);
	lightdir = normalize(lightdir);
	float3 lightcolor = float3(1, 1, 1);
	float3 amb = ambient;
	float3 diff = dot(-lightdir, normal) * diffuse;
	const float3 eye = float3(0, 0, -20);
	const float shininess = 4.0f;
	float3 eyedir = normalize(eye - pos.xyz);
	float3 reflect = normalize(lightdir + 2 * dot(-lightdir, normal) * normal);
	float3 spe = pow(saturate(dot(reflect, eyedir)), shininess) * specular;
	VSOutput output;//ピクセルシェーダーに渡す値
	output.svpos = mul(mat, pos);	//座標に行列を乗算
	output.color.rgb = (amb + diff + spe) * lightcolor;
	output.color.a = alpha;
	output.uv = uv;
	return output;
}