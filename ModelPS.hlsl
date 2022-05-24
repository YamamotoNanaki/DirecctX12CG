#include "Model.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 main(GSOutput input) : SV_TARGET
{
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	float4 shadecolor;
	const float shininess = 4.0f;
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);
	float3 dotlightnormal = dot(lightv, input.normal);
	float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
	float3 amb = ambient;
	float3 diff = dotlightnormal * diffuse;
	float3 spe = pow(saturate(dot(reflect, eyedir)), shininess) * specular;
	shadecolor.rgb = (amb + diff + spe) * lightcolor;
	shadecolor.a = alpha;
	return shadecolor * texcolor;
}