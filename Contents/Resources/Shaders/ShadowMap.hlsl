
float normal_offset = 0.001f;

cbuffer cbShadow : register(b0)
{
	row_major matrix view : packoffset(c0);
	row_major matrix proj : packoffset(c4);
	row_major matrix light_view : packoffset(c8);
	row_major matrix light_proj : packoffset(c12);
	float3 eye_pos : packoffset(c16);
	float3 dir_light_pos : packoffset(c19);
	int timer : packoffset(c22);
}

cbuffer cbMesh: register(b1)
{
	row_major matrix world    : packoffset(c0);
	float4 color    : packoffset(c4);
}

struct VS_INPUT
{
	float3	pos			: POSITION;
};

struct VS_OUTPUT
{
	float4	pos			: SV_POSITION;
	float4	depth		: TEXCOORD;
};

VS_OUTPUT VSMain(VS_INPUT vs_in)
{
	VS_OUTPUT output = (VS_OUTPUT) 0;

	matrix WVP = mul(world, mul(light_view, light_proj));

	output.pos = mul(float4(vs_in.pos, 1.f), WVP);
	output.depth = output.pos;

	return output;
}

float PSMain(VS_OUTPUT ps_in) : SV_TARGET
{
	float depth = ps_in.depth.z / ps_in.depth.w;
	return  depth;
}