Texture2D diffuse_texture : register(t0);
SamplerState diffuse_sampler : register(s0);

cbuffer cbMatrix : register(b0)
{
	row_major float4x4 World : packoffset(c0);
	row_major float4x4 View : packoffset(c4);
	row_major float4x4 Projection : packoffset(c8);
};

struct VS_INPUT
{
	float4 Position   : POSITION;
	float4 Color      : COLOR;
	float2 Tex		  : TEXCOORD;
	float3 Normal	  : NORMAL;
	float3 Tangent	  : TANGENT;
};

struct VS_OUTPUT
{
	float4 Position   : SV_POSITION;
	float2 Tex		  : TEXCOORD;
};

VS_OUTPUT VSMain(VS_INPUT In)
{
	VS_OUTPUT Output;
	Output.Position = mul(In.Position, World);
	Output.Position = mul(Output.Position, View);
	Output.Position = mul(Output.Position, Projection);
	Output.Tex = In.Tex;

	return Output;
}

float4 PSMain(VS_OUTPUT In) : SV_TARGET
{
	return diffuse_texture.Sample(diffuse_sampler, In.Tex) * float4(1.0f, 1.0f, 1.0f, 1.0f);
}