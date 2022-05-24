#include "Model.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	float4 wnormal = normalize(mul(world, float4(normal, 0)));
	float4 wpos = mul(world, pos);

	float3 amb = ambient;
	const float shininess = 4.0f;
	float3 eyedir = normalize(cameraPos - wpos.xyz);
	float3 diff = dot(lightv, wnormal.xyz) * diffuse;
	float3 reflect = normalize(-lightv + 2 * dot(lightv, wnormal.xyz) * wnormal.xyz);
	float3 spe = pow(saturate(dot(reflect, eyedir)), shininess) * specular;
	VSOutput output;//ピクセルシェーダーに渡す値
	output.svpos = mul(mul(viewPro, world), pos);	//座標に行列を乗算
	output.color.rgb = (amb + diff + spe) * lightcolor;
	output.color.a = alpha;
	output.uv = uv;
	return output;
}