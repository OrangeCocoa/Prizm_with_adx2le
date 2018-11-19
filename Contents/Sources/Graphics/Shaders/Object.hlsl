
Texture2D diffuse_texture : register(t0);
Texture2D shadow_map_texture : register(t1);

SamplerState diffuse_sampler : register(s0);
SamplerComparisonState shadow_sampler : register(s1);

cbuffer cbMatrix : register(b0)
{
	row_major float4x4 World      : packoffset(c0);
	row_major float4x4 View       : packoffset(c4);
	row_major float4x4 Projection : packoffset(c8);
	float4			   CameraPos  : packoffset(c12);
	float4			   LightDir   : packoffset(c13);
	row_major float4x4 Shadow     : packoffset(c14);
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
	float4 Position  : SV_POSITION;
	float4 WorldPos  : WORLD_POSITION;
	float3 Normal    : NORMAL;
	float2 Tex		 : TEXCOORD;
	float3 LightDir  : LIGHT_DIRECTION;
	float3 ViewDir   : VIEW_DIRECTION;
	float4 SdwCoord  : SHADOW_COORD;
};

VS_OUTPUT VSMain(VS_INPUT In)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	float4 local_pos = float4(In.Position.xyz, 1.0f);
	float4 world_pos = mul(World, local_pos);
	float4 view_pos = mul(View, world_pos);
	float4 proj_pos = mul(Projection, view_pos);

	Output.Position = proj_pos;
	Output.WorldPos = world_pos;
	Output.LightDir = -LightDir.xyz;
	Output.ViewDir = world_pos.xyz - CameraPos.xyz;

	Output.Normal = In.Normal;
	Output.Tex = In.Tex;

	Output.SdwCoord = mul(Shadow, world_pos);

	return Output;
}

float4 PSMain(VS_OUTPUT In) : SV_TARGET
{
	// シャドウマップの深度値と比較する.
	float3 shadow_coord = In.SdwCoord.xyz / In.SdwCoord.w;

	// 最大深度傾斜を求める.
	float  max_depth_slope = max(abs(ddx(shadow_coord.z)), abs(ddy(shadow_coord.z)));

	float  shadow_threshold = 1.0f;      // シャドウにするかどうかの閾値.
	float  bias = 0.01f;				// 固定バイアス.
	float  slope_scaled_bias = 0.01f;     // 深度傾斜.
	float  depth_bias_clamp = 0.1f;       // バイアスクランプ値.

	float  shadow_bias = bias + slope_scaled_bias * max_depth_slope;
	shadow_bias = min(shadow_bias, depth_bias_clamp);

	float3 shadow_color = float3(0.25f, 0.25f, 0.25f);
	shadow_threshold = shadow_map_texture.SampleCmpLevelZero(shadow_sampler, shadow_coord.xy, shadow_coord.z - shadow_bias);
	shadow_color = lerp(shadow_color, float3(1.0f, 1.0f, 1.0f), shadow_threshold);

	return diffuse_texture.Sample(diffuse_sampler, In.Tex) * float4(shadow_color, 1.0f);
}