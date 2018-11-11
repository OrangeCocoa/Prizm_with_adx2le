// ライトのタイル処理

#include "LightBuffer.hlsli"

#define kTileWidth				(16)
#define kTileSize				(kTileWidth * kTileWidth)
#define kMaxLight				(1024)

cbuffer cbCameraParam : register( b0 )
{
	float4	mtxView[3]			: packoffset( c0 );		// ビュー行列
	float4	mtxProj[4]			: packoffset( c3 );		// 投影行列
	float4	screenParam			: packoffset( c7 );		// (near, far, screenWidth, screenHeight)
	float4	frustumCorner		: packoffset( c8 );		// ビューフラスタムの角へのベクトル
	float4	mtxLastView[3]		: packoffset( c9 );		// 前回のビュー行列
	float4	mtxLastProj[4]		: packoffset( c12 );	// 前回の投影行列
};

struct PointLight
{
	float3	pos;
	float4	color;
	float4	attn;
};

struct SurfaceData
{
	float3	posView;
	float3	normalView;
	float3	albedo;
	float	specular;
};

// 出力構造体
struct PointVertexGS
{
	float4	pos;
	float4	rect;			// (minX, maxX, minY, maxY)
	float	depth;
	float4	color;
	float4	attn;			// (attn, ?, ?, spec_power)
};

// 入力
StructuredBuffer<PointLight>	rLightBuffer	: register( t0 );
Texture2D						rNormalDepthTex	: register( t1 );
Texture2D						rAlbedoTex		: register( t2 );
Texture2D						rShadowTex		: register( t3 );
Texture2D						rHdaoTex		: register( t4 );

// 出力
RWStructuredBuffer<uint>		rwLightIndices	: register( u0 );		// Forwardの場合のライトインデックスバッファ

// 共有メモリ
groupshared uint sMinZ;		// タイルの最小深度
groupshared uint sMaxZ;		// タイルの最大深度
groupshared uint sTileLightIndices[kMaxLight];	// タイルに接触しているポイントライトのインデックス
groupshared uint sTileNumLights;				// タイルに接触しているポイントライトの数
groupshared uint sPerSamplePixels[kTileSize];
groupshared uint sNumPerSamplePixels;

//! サーフェイス情報を取得する
SurfaceData GetSurfaceData( uint2 uv )
{
	float4 nd = rNormalDepthTex[uv];
	float4 as = rAlbedoTex[uv];

	// ビュー空間での座標を求める
	float2	st = ((float2)uv + 0.5) * rcp( screenParam.zw );
	st = st * float2( 2.0, -2.0 ) - float2( 1.0, -1.0 );
	float3	view = { frustumCorner.x * st.x, frustumCorner.y * st.y, -frustumCorner.z };
	float3	pos = nd.w * view;

	SurfaceData ret = 
	{
		pos,
		nd.xyz,
		as.xyz,
		as.w
	};
	return ret;
}

//! タイルの錘台を求める
void GetTileFrustumPlane( out float4 frustumPlanes[6], uint3 groupId )
{
	// タイルの最大・最小深度を浮動小数点に変換
	float minTileZ = asfloat(sMinZ);
	float maxTileZ = asfloat(sMaxZ);

	// Intelのサンプルと微妙に違うのは右手系でやっているから
	// あと、Intelのサンプルは微妙に間違ってると思う
	float2 tileScale = screenParam.zw * rcp( float(2 * kTileWidth) );
	float2 tileBias = tileScale - float2(groupId.xy);

	float4 c1 = float4(mtxProj[0].x * tileScale.x, 0.0, -tileBias.x, 0.0);
	float4 c2 = float4(0.0, -mtxProj[1].y * tileScale.y, -tileBias.y, 0.0);
	float4 c4 = float4(0.0, 0.0, -1.0, 0.0);

	frustumPlanes[0] = c4 - c1;		// Right
	frustumPlanes[1] = c1;			// Left
	frustumPlanes[2] = c4 - c2;		// Top
	frustumPlanes[3] = c2;			// Bottom
	frustumPlanes[4] = float4(0.0, 0.0, -1.0, -minTileZ);
	frustumPlanes[5] = float4(0.0, 0.0, 1.0, maxTileZ);

	// 法線が正規化されていない4面についてだけ正規化する
	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
		frustumPlanes[i] *= rcp( length( frustumPlanes[i].xyz ) );
	}
}

//! コンピュートシェーダ
[numthreads(kTileWidth, kTileWidth, 1)]
void CSMain(
	uint3 groupId          : SV_GroupID,
	uint3 dispatchThreadId : SV_DispatchThreadID,
	uint3 groupThreadId    : SV_GroupThreadID )
{
	// タイル内でのインデックスを求める
    uint groupIndex = groupThreadId.y * kTileWidth + groupThreadId.x;

	// ライトの数を取得する
    uint numLights, dummy;
    rLightBuffer.GetDimensions( numLights, dummy );

	// 各ピクセルの法線、深度、アルベドを取得する
	uint2 frameUV = dispatchThreadId.xy;
	SurfaceData surface = GetSurfaceData( frameUV );

	// 共有メモリを初期化する
	if (groupIndex == 0)
	{
		sTileNumLights = 0;
		sNumPerSamplePixels = 0;
		sMinZ = 0x7F7FFFFF;		// floatの最大値
		sMaxZ = 0;
	}

	// ここで同期を取る
    GroupMemoryBarrierWithGroupSync();

	// タイルの最大・最小深度を求める
	// この処理は並列するスレッド全てで排他的に処理される
	InterlockedMin( sMinZ, asuint(-surface.posView.z) );
	InterlockedMax( sMaxZ, asuint(-surface.posView.z) );

	// ここで同期を取ることでタイルの最大・最小深度を正しいものにする
    GroupMemoryBarrierWithGroupSync();

	// タイルの錘台を求める
	float4 frustumPlanes[6];
	GetTileFrustumPlane( frustumPlanes, groupId );

	// タイルとポイントライトの衝突判定
	for (uint lightIndex = groupIndex; lightIndex < numLights; lightIndex += kTileSize)
	{
		PointLight light = rLightBuffer[lightIndex];

		// タイルとの判定
		bool inFrustum = true;
		[unroll]
		for (uint i = 0; i < 6; ++i)
		{
			float4 lp = float4( light.pos, 1.0 );
			float d = dot( frustumPlanes[i], lp );
			inFrustum = inFrustum && (d >= -light.attn.x);
		}

		// タイルと衝突している場合
		[branch]
		if (inFrustum)
		{
			uint listIndex;
			InterlockedAdd( sTileNumLights, 1, listIndex );
			sTileLightIndices[listIndex] = lightIndex;
		}
	}

	// ここで同期を取ると、sTileLightIndicesにタイルと衝突しているライトのインデックスが積まれている
    GroupMemoryBarrierWithGroupSync();

	// ライトインデックスを出力バッファに出力
	uint numCellX = (screenParam.z + kTileWidth - 1) / kTileWidth;
	uint tileIndex = floor( frameUV.x / kTileWidth ) + floor( frameUV.y / kTileWidth ) * numCellX;
	uint lightStart = numLights * tileIndex;
	for (uint lightIndex = groupIndex; lightIndex < sTileNumLights; lightIndex += kTileSize)
	{
		rwLightIndices[lightStart + lightIndex] = sTileLightIndices[lightIndex];
	}
	[branch]
	if ((groupIndex == 0) && (sTileNumLights < numLights))
	{
		rwLightIndices[lightStart + sTileNumLights] = 0xffffffff;
	}
}