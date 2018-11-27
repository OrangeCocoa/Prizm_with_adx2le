Texture2D diffuse_texture : register(t0);
SamplerState diffuse_sampler : register(s0);

struct VS_INPUT
{
	float2 Position   : POSITION;
	float4 Color      : COLOR;
	float2 Tex		  : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position   : SV_POSITION;
	float4 Color      : COLOR;
	float2 Tex		  : TEXCOORD;
};

VS_OUTPUT VSMain(VS_INPUT In)
{
	VS_OUTPUT Output;
	Output.Position = float4(In.Position.x, In.Position.y, 0, 1);
	Output.Color = In.Color;
	Output.Tex = In.Tex;

	return Output;
}

float4 PSMain(VS_OUTPUT In) : SV_TARGET
{
	return diffuse_texture.Sample(diffuse_sampler, In.Tex) * In.Color;
}