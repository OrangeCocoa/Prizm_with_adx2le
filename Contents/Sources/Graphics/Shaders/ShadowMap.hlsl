
float normal_offset = 0.001f;

cbuffer cbShadow : register(b0)
{
	float4x4 mtx_shadow_VP : packoffset(c0);
}

cbuffer cbMesh: register(b1)
{
	float4x4 mtx_world    : packoffset(c0);
	float4x4 mtx_world_it : packoffset(c4);
}

struct VS_INPUT
{
	float4	pos			: POSITION;
	float3	normal		: NORMAL;
};

struct VS_OUTPUT
{
	float4	pos			: SV_POSITION;
	float2	depth		: TEXCOORD;
};

VS_OUTPUT VSMain(VS_INPUT vs_in)
{
	VS_OUTPUT vs_out;

	// ç¿ïWåvéZ
	float4	pos = float4(vs_in.pos.xyz - vs_in.normal.xyz * normal_offset, 1.0);
	float4	pos_world = float4(
		dot(mtx_world[0], pos),
		dot(mtx_world[1], pos),
		dot(mtx_world[2], pos), 1.0);
	
	vs_out.pos.x = dot(mtx_shadow_VP[0], pos_world);
	vs_out.pos.y = dot(mtx_shadow_VP[1], pos_world);
	vs_out.pos.z = dot(mtx_shadow_VP[2], pos_world);
	vs_out.pos.w = dot(mtx_shadow_VP[3], pos_world);

	vs_out.depth.xy = vs_out.pos.zw;

	return vs_out;
}

float4 PSMain(VS_OUTPUT ps_in) : SV_TARGET
{
	float	depth = ps_in.depth.x / ps_in.depth.y;
	return float4(depth, depth, depth, 1.0);
}