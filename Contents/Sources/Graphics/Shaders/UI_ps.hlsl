Texture2D diffuse_texture : register(t0);
SamplerState diffuse_sampler : register(s0);

struct VS_OUTPUT
{
	float2 Position   : POSITION;
	float4 Color      : COLOR;
	float2 Tex		  : TEXCOORD;
};

float4 PSMain(VS_OUTPUT In) : SV_TARGET
{
	return diffuse_texture.Sample(diffuse_sampler, In.Tex) * In.Color;
}