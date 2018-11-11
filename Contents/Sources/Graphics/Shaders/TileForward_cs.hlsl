// ���C�g�̃^�C������

#include "LightBuffer.hlsli"

#define kTileWidth				(16)
#define kTileSize				(kTileWidth * kTileWidth)
#define kMaxLight				(1024)

cbuffer cbCameraParam : register( b0 )
{
	float4	mtxView[3]			: packoffset( c0 );		// �r���[�s��
	float4	mtxProj[4]			: packoffset( c3 );		// ���e�s��
	float4	screenParam			: packoffset( c7 );		// (near, far, screenWidth, screenHeight)
	float4	frustumCorner		: packoffset( c8 );		// �r���[�t���X�^���̊p�ւ̃x�N�g��
	float4	mtxLastView[3]		: packoffset( c9 );		// �O��̃r���[�s��
	float4	mtxLastProj[4]		: packoffset( c12 );	// �O��̓��e�s��
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

// �o�͍\����
struct PointVertexGS
{
	float4	pos;
	float4	rect;			// (minX, maxX, minY, maxY)
	float	depth;
	float4	color;
	float4	attn;			// (attn, ?, ?, spec_power)
};

// ����
StructuredBuffer<PointLight>	rLightBuffer	: register( t0 );
Texture2D						rNormalDepthTex	: register( t1 );
Texture2D						rAlbedoTex		: register( t2 );
Texture2D						rShadowTex		: register( t3 );
Texture2D						rHdaoTex		: register( t4 );

// �o��
RWStructuredBuffer<uint>		rwLightIndices	: register( u0 );		// Forward�̏ꍇ�̃��C�g�C���f�b�N�X�o�b�t�@

// ���L������
groupshared uint sMinZ;		// �^�C���̍ŏ��[�x
groupshared uint sMaxZ;		// �^�C���̍ő�[�x
groupshared uint sTileLightIndices[kMaxLight];	// �^�C���ɐڐG���Ă���|�C���g���C�g�̃C���f�b�N�X
groupshared uint sTileNumLights;				// �^�C���ɐڐG���Ă���|�C���g���C�g�̐�
groupshared uint sPerSamplePixels[kTileSize];
groupshared uint sNumPerSamplePixels;

//! �T�[�t�F�C�X�����擾����
SurfaceData GetSurfaceData( uint2 uv )
{
	float4 nd = rNormalDepthTex[uv];
	float4 as = rAlbedoTex[uv];

	// �r���[��Ԃł̍��W�����߂�
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

//! �^�C���̐�������߂�
void GetTileFrustumPlane( out float4 frustumPlanes[6], uint3 groupId )
{
	// �^�C���̍ő�E�ŏ��[�x�𕂓������_�ɕϊ�
	float minTileZ = asfloat(sMinZ);
	float maxTileZ = asfloat(sMaxZ);

	// Intel�̃T���v���Ɣ����ɈႤ�͉̂E��n�ł���Ă��邩��
	// ���ƁAIntel�̃T���v���͔����ɊԈ���Ă�Ǝv��
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

	// �@�������K������Ă��Ȃ�4�ʂɂ��Ă������K������
	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
		frustumPlanes[i] *= rcp( length( frustumPlanes[i].xyz ) );
	}
}

//! �R���s���[�g�V�F�[�_
[numthreads(kTileWidth, kTileWidth, 1)]
void CSMain(
	uint3 groupId          : SV_GroupID,
	uint3 dispatchThreadId : SV_DispatchThreadID,
	uint3 groupThreadId    : SV_GroupThreadID )
{
	// �^�C�����ł̃C���f�b�N�X�����߂�
    uint groupIndex = groupThreadId.y * kTileWidth + groupThreadId.x;

	// ���C�g�̐����擾����
    uint numLights, dummy;
    rLightBuffer.GetDimensions( numLights, dummy );

	// �e�s�N�Z���̖@���A�[�x�A�A���x�h���擾����
	uint2 frameUV = dispatchThreadId.xy;
	SurfaceData surface = GetSurfaceData( frameUV );

	// ���L������������������
	if (groupIndex == 0)
	{
		sTileNumLights = 0;
		sNumPerSamplePixels = 0;
		sMinZ = 0x7F7FFFFF;		// float�̍ő�l
		sMaxZ = 0;
	}

	// �����œ��������
    GroupMemoryBarrierWithGroupSync();

	// �^�C���̍ő�E�ŏ��[�x�����߂�
	// ���̏����͕��񂷂�X���b�h�S�ĂŔr���I�ɏ��������
	InterlockedMin( sMinZ, asuint(-surface.posView.z) );
	InterlockedMax( sMaxZ, asuint(-surface.posView.z) );

	// �����œ�������邱�ƂŃ^�C���̍ő�E�ŏ��[�x�𐳂������̂ɂ���
    GroupMemoryBarrierWithGroupSync();

	// �^�C���̐�������߂�
	float4 frustumPlanes[6];
	GetTileFrustumPlane( frustumPlanes, groupId );

	// �^�C���ƃ|�C���g���C�g�̏Փ˔���
	for (uint lightIndex = groupIndex; lightIndex < numLights; lightIndex += kTileSize)
	{
		PointLight light = rLightBuffer[lightIndex];

		// �^�C���Ƃ̔���
		bool inFrustum = true;
		[unroll]
		for (uint i = 0; i < 6; ++i)
		{
			float4 lp = float4( light.pos, 1.0 );
			float d = dot( frustumPlanes[i], lp );
			inFrustum = inFrustum && (d >= -light.attn.x);
		}

		// �^�C���ƏՓ˂��Ă���ꍇ
		[branch]
		if (inFrustum)
		{
			uint listIndex;
			InterlockedAdd( sTileNumLights, 1, listIndex );
			sTileLightIndices[listIndex] = lightIndex;
		}
	}

	// �����œ��������ƁAsTileLightIndices�Ƀ^�C���ƏՓ˂��Ă��郉�C�g�̃C���f�b�N�X���ς܂�Ă���
    GroupMemoryBarrierWithGroupSync();

	// ���C�g�C���f�b�N�X���o�̓o�b�t�@�ɏo��
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