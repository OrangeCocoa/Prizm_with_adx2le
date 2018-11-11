// 法線・深度のエンコード・デコード関数

/*
// Lambert Azimuthal F32バッファ用
float4 EncodeNormalDepthLambertF32( float3 normal, float depth );
void DecodeNormalDepthLambertF32( out float3 normal, out float depth, float4 input );

// Lambert Azimuthal RGBA8バッファ用
float4 EncodeNormalDepthLambert8888( float3 normal, float depth );
void DecodeNormalDepthLambert8888( out float3 normal, out float depth, float4 input );

// Stereographic Projection F32バッファ用
float4 EncodeNormalDepthStereographicF32( float3 normal, float depth );
void DecodeNormalDepthStereographicF32( out float3 normal, out float depth, float4 input );

// Stereographic Projection RGBA8バッファ用
float4 EncodeNormalDepthStereographic8888( float3 normal, float depth );
void DecodeNormalDepthStereographic8888( out float3 normal, out float depth, float4 input );
*/

// Lambert Azimuthal Equal
float2 EncodeNormalLambert( float3 normal )
{
	float	f = sqrt( 8.0 * normal.z + 8.0 );
	return normal.xy / f + 0.5;
}
float3 DecodeNormalLambert( float2 input )
{
	float2	fenc = input * 4.0 - 2.0;
	float	f = dot( fenc, fenc );
	float	g = sqrt( 1.0 - f / 4.0 );
	float3	ret;
	ret.xy = fenc * g;
	ret.z  = 1.0 - f / 2.0;
	return ret;
}

// Stereographic Projection
float2 EncodeNormalStereographic( float3 normal )
{
	float	scale = 1.7777;
	float2	ret = normal.xy / (normal.z + 1.0);
	ret /= scale;
	return ret * 0.5 + 0.5;
}
float3 DecodeNormalStereographic( float2 input )
{
	float	scale = 1.7777;
	float3	nn = float3(input, 0.0) * float3(2.0*scale, 2.0*scale, 0.0) + float3(-scale, -scale, 1);
	float	g = 2.0 / dot(nn.xyz, nn.xyz);
	nn.xy = g * nn.xy;
	nn.z  = g - 1.0;
	return nn;
}

// Depth Encode/Decode
float2 EncodeDepth16( float depth )
{
	float	d = depth * 256.0;
	return float2( floor(d) / 256.0, frac(d) );
}
float DecodeDepth16( float2 input )
{
	return input.x + input.y / 256.0;
}

// Normal by Lambert and 32bit Depth
float4 EncodeNormalDepthLambertF32( float3 normal, float depth )
{
	float4	ret = 0;
	ret.xy = EncodeNormalLambert( normal );
	ret.z  = depth;
	return ret;
}
void DecodeNormalDepthLambertF32( out float3 normal, out float depth, float4 input )
{
	normal = DecodeNormalLambert( input.xy );
	depth  = input.z;
}

// Normal by Lambert and 16bit Depth
float4 EncodeNormalDepthLambert8888( float3 normal, float depth )
{
	float4	ret = 0;
	ret.xy = EncodeNormalLambert( normal );
	ret.zw = EncodeDepth16( depth );
	return ret;
}

void DecodeNormalDepthLambert8888( out float3 normal, out float depth, float4 input )
{
	normal = DecodeNormalLambert( input.xy );
	depth  = DecodeDepth16( input.zw );
}

// Normal by Stereoprahic and 32bit Depth
float4 EncodeNormalDepthStereographicF32( float3 normal, float depth )
{
	float4	ret = 0;
	ret.xy = EncodeNormalStereographic( normal );
	ret.z  = depth;
	return ret;
}
void DecodeNormalDepthStereographicF32( out float3 normal, out float depth, float4 input )
{
	normal = DecodeNormalStereographic( input.xy );
	depth  = input.z;
}

// Normal by Stereoprahic and 16bit Depth
float4 EncodeNormalDepthStereographic8888( float3 normal, float depth )
{
	float4	ret = 0;
	ret.xy = EncodeNormalStereographic( normal );
	ret.zw = EncodeDepth16( depth );
	return ret;
}

void DecodeNormalDepthStereographic8888( out float3 normal, out float depth, float4 input )
{
	normal = DecodeNormalStereographic( input.xy );
	depth  = DecodeDepth16( input.zw );
}

