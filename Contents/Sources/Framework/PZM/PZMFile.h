#pragma once

#include<string>
#include<vector>
#include<DirectXTK\SimpleMath.h>

namespace Prizm
{
	struct PZMHeader
	{
		unsigned char	data_size;

		unsigned char	add_UV_num;
		unsigned char	vertex_index_size;
		unsigned char	texture_index_size;
		unsigned char	material_index_size;
		unsigned char	bone_index_size;
		unsigned char	morph_index_size;
		unsigned char	rigidbody_index_size;
	};

	struct PZMInfo
	{
		std::string model_name;
		std::string comment;
	};

	struct PZMVertex
	{
		DirectX::SimpleMath::Vector3	position;
		DirectX::SimpleMath::Vector3	normal;
		DirectX::SimpleMath::Vector2	uv;

		DirectX::SimpleMath::Vector4	add_UV[4];

		/*
		BDEF1
		boneIndices[0]

		BDEF2
		boneIndices[0-1]
		boneWeights[0]

		BDEF4
		boneIndices[0-3]
		boneWeights[0-3]

		SDEF
		boneIndices[0-1]
		boneWeights[0]
		sdefC
		sdefR0
		sdefR1

		QDEF
		boneIndices[0-3]
		boneWeights[0-3]
		*/
		enum class VertexWeight : unsigned char
		{
			BDEF1,
			BDEF2,
			BDEF4,
			SDEF,
			QDEF,
		};

		VertexWeight					weight_type; // 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF 4:QDEF
		int								bone_indices[4];
		float							bone_weights[4];
		DirectX::SimpleMath::Vector3	sdef_C;
		DirectX::SimpleMath::Vector3	sdef_R0;
		DirectX::SimpleMath::Vector3	sdef_R1;

		float	edge_mag;
	};

	struct PZMFace
	{
		int	indices[3];
	};

	struct PZMTexture
	{
		std::string texture_name;
	};

	struct PZMMaterial
	{
		std::string name;

		DirectX::SimpleMath::Vector4	diffuse;
		DirectX::SimpleMath::Vector3	specular;
		float							specular_power;
		DirectX::SimpleMath::Vector3	ambient;

		/*
		0x01:両面描画
		0x02:地面影
		0x04:draw to self shadow map
		0x08:draw self shadow
		0x10:draw edge
		0x20:頂点カラー(※2.1 extension)
		0x40:Point描画(※2.1 extension)
		0x80:Line描画(※2.1 extension)
		*/
		enum class DrawModeFlags : unsigned char
		{
			BothFace = 0x01,
			GroundShadow = 0x02,
			CastSelfShadow = 0x04,
			RecieveSelfShadow = 0x08,
			DrawEdge = 0x10,
			VertexColor = 0x20,
			DrawPoint = 0x40,
			DrawLine = 0x80,
		};

		enum class SphereMode : unsigned char
		{
			None,
			Mul,
			Add,
			SubTexture,
		};

		enum class ToonMode : unsigned char
		{
			Separate = 0,	// 0:unique toon
			Common,			// 1:shared toon[0-9] toon01.bmp～toon10.bmp
		};

		DrawModeFlags				draw_mode;

		DirectX::SimpleMath::Vector4	edge_color;
		float							edge_size;

		int				texture_index;
		int				sphere_texture_index;
		SphereMode	sphere_mode;

		ToonMode	toon_mode;
		int			toon_texture_index;

		int	num_face_vertices;
	};

	/*
	0x0001  : 接続先(PMD子ボーン指定)表示方法 -> 0:座標オフセットで指定 1:ボーンで指定

	0x0002  : 回転可能
	0x0004  : 移動可能
	0x0008  : 表示
	0x0010  : 操作可

	0x0020  : IK

	0x0080  : ローカル付与 | 付与対象 0:ユーザー変形値／IKリンク／多重付与 1:親のローカル変形量
	0x0100  : 回転付与
	0x0200  : 移動付与

	0x0400  : 軸固定
	0x0800  : ローカル軸

	0x1000  : 物理後変形
	0x2000  : 外部親変形
	*/

	struct PZMBone
	{
		std::string name;

		enum class BoneFlags : unsigned short
		{
			TargetShowMode = 0x0001,
			AllowRotate = 0x0002,
			AllowTranslate = 0x0004,
			Visible = 0x0008,
			AllowControl = 0x0010,
			IK = 0x0020,
			AppendLocal = 0x0080,
			AppendRotate = 0x0100,
			AppendTranslate = 0x0200,
			FixedAxis = 0x0400,
			LocalAxis = 0x0800,
			DeformAfterPhysics = 0x1000,
			DeformOuterParent = 0x2000,
		};

		DirectX::SimpleMath::Vector3	position;
		int								parent_bone_index;
		int								deform_depth;

		BoneFlags					bone_flag; // link bone enable

		DirectX::SimpleMath::Vector3	position_offset;	// bone frag : false
		int								link_bone_index;	// bone_flag : true

															// [AppendRotate] or [AppendTranslate] enabled only
		int		append_bone_index;
		float	append_weight;

		// [FixedAxis] enabled only
		DirectX::SimpleMath::Vector3	fixed_axis;

		// [LocalAxis] enabled only
		DirectX::SimpleMath::Vector3	local_X_axis;
		DirectX::SimpleMath::Vector3	local_Z_axis;

		// [DeformOuterParent] enabled only
		int	key_value;

		// [IK] enabled only
		int		ik_target_bone_index;
		int		ik_iteration_count;
		float	ik_limit;	// radian

		struct IKLink
		{
			int				ik_bone_index;
			unsigned char	enable_limit;

			//enable_limit is only 1
			DirectX::SimpleMath::Vector3 limit_min;	// radian
			DirectX::SimpleMath::Vector3 limit_max;	// radian
		};

		std::vector<IKLink>	ik_links;
	};

	struct PZMMorph
	{
		std::string name;

		enum class MorphType : unsigned char
		{
			Group = 0,
			Position,
			Bone,
			UV,
			AddUV1,
			AddUV2,
			AddUV3,
			AddUV4,
			Material,
			Flip,		// (※2.1 extension)
			Impulse,	// (※2.1 extension)
		};

		unsigned char	control_panel;	//1 : eyeblow(left bottom) 2 : eye(left up) 3 : mouth(right up) 4 : other(right bottom)  | 0 : system reserve
		MorphType		morph_type;

		struct PositionMorph
		{
			int								vertex_index;
			DirectX::SimpleMath::Vector3	position;
		};

		struct UVMorph
		{
			int								vertex_index;
			DirectX::SimpleMath::Vector4	uv;
		};

		struct BoneMorph
		{
			int								bone_index;
			DirectX::SimpleMath::Vector3	position;
			DirectX::SimpleMath::Quaternion	quaternion;
		};

		struct MaterialMorph
		{
			enum class OpType : unsigned char
			{
				Mul,
				Add,
			};

			int							    material_index;
			OpType							op_type;	//0:multiple 1:additive
			DirectX::SimpleMath::Vector4	diffuse;
			DirectX::SimpleMath::Vector3	specular;
			float							specular_power;
			DirectX::SimpleMath::Vector3	ambient;
			DirectX::SimpleMath::Vector4	edge_color;
			float							edge_size;
			DirectX::SimpleMath::Vector4	texture_factor;
			DirectX::SimpleMath::Vector4	sphere_texture_factor;
			DirectX::SimpleMath::Vector4	toon_texture_factor;
		};

		struct GroupMorph
		{
			int		morph_index;
			float	weight;
		};

		struct FlipMorph
		{
			int		morph_index;
			float	weight;
		};

		struct ImpulseMorph
		{
			int								rigidbody_index;
			unsigned char					local_flag;	//0:OFF 1:ON
			DirectX::SimpleMath::Vector3	translate_velocity;
			DirectX::SimpleMath::Vector3	rotate_torque;
		};

		std::vector<PositionMorph>	position_morph;
		std::vector<UVMorph>		uv_morph;
		std::vector<BoneMorph>		bone_morph;
		std::vector<MaterialMorph>	material_morph;
		std::vector<GroupMorph>		group_morph;
		std::vector<FlipMorph>		flip_morph;
		std::vector<ImpulseMorph>	impulse_morph;
	};

	struct PZMRigidbody
	{
		std::string name;

		int				bone_index;
		unsigned char	group;
		unsigned short	collision_group;

		enum class Shape : unsigned char
		{
			Sphere = 0,
			Box,
			Capsule,
		};

		Shape							shape;
		DirectX::SimpleMath::Vector3	shape_size;

		DirectX::SimpleMath::Vector3	translate;
		DirectX::SimpleMath::Vector3	rotate;	// radian

		float	mass;
		float	translate_dimmer;
		float	rotate_dimmer;
		float	repulsion;
		float	friction;

		/*
		0:bone accompany(static)
		1:physics calculate(dynamic)
		2:physics calculate + bone position merge
		*/
		enum class Operation : unsigned char
		{
			Static = 0,
			Dynamic,
			DynamicAndBoneMerge
		};
		Operation	op;
	};

	struct PZMJoint
	{
		std::string name;

		enum class JointType : unsigned char
		{// PZM 2.0 used only SpringDOF6. Other factor is PZM 2.1 extension. Same all data.
			SpringDOF6 = 0,
			DOF6,
			P2P,
			ConeTwist,
			Slider,
			Hinge,
		};

		JointType						type;
		int								rigidbody_A_index;
		int								rigidbody_B_index;

		DirectX::SimpleMath::Vector3	translate;
		DirectX::SimpleMath::Vector3	rotate;

		DirectX::SimpleMath::Vector3	translate_lower_limit;
		DirectX::SimpleMath::Vector3	translate_upper_limit;
		DirectX::SimpleMath::Vector3	rotate_lower_limit;
		DirectX::SimpleMath::Vector3	rotate_upper_limit;

		DirectX::SimpleMath::Vector3	spring_translate_factor;
		DirectX::SimpleMath::Vector3	spring_rotate_factor;
	};

	struct PZMSoftbody
	{// PZM 2.1 additional extention
		std::string name;

		enum class SoftbodyType : unsigned char
		{
			TriMesh = 0,
			Rope,
		};
		SoftbodyType	type;

		int			material_index;

		unsigned char	group;
		unsigned short	collision_group;

		/*
		0x01 : B-Link create
		0x02 : Cluster create
		0x04 : link hybrid
		*/
		enum class SoftbodyMask : unsigned char
		{
			BLink = 0x01,
			Cluster = 0x02,
			HybridLink = 0x04,
		};
		SoftbodyMask	flag;

		int	B_link_length;
		int	num_clusters;

		float	total_mass;
		float	collision_margin;

		enum class AeroModel : int
		{
			AeroModelV_TwoSided = 0,
			AeroModelV_OneSided,
			AeroModelF_TwoSided,
			AeroModelF_OneSided,
		};
		int		aero_model;

		//config
		float	VCF;
		float	DP;
		float	DG;
		float	LF;
		float	PR;
		float	VC;
		float	DF;
		float	MT;
		float	CHR;
		float	KHR;
		float	SHR;
		float	AHR;

		//cluster
		float	SRHR_CL;
		float	SKHR_CL;
		float	SSHR_CL;
		float	SR_SPLT_CL;
		float	SK_SPLT_CL;
		float	SS_SPLT_CL;

		//interation
		int	V_IT;
		int	P_IT;
		int	D_IT;
		int	C_IT;

		//material
		float	LST;
		float	AST;
		float	VST;

		struct AnchorRigidbody
		{
			int				rigid_body_index;
			int				vertex_index;
			unsigned char	near_mode; //0:OFF 1:ON
		};
		std::vector<AnchorRigidbody> anchor_rigidbodies;

		std::vector<int> pin_vertex_indices;
	};

	struct PZMFile
	{
		PZMHeader	header;
		PZMInfo		info;

		std::vector<PZMVertex>			vertices;
		std::vector<PZMFace>			faces;
		std::vector<PZMTexture>			textures;
		std::vector<PZMMaterial>		materials;
		std::vector<PZMBone>			bones;
		std::vector<PZMMorph>			morphs;
		std::vector<PZMRigidbody>		rigidbodies;
		std::vector<PZMJoint>			joints;
		std::vector<PZMSoftbody>		softbodies;
	};

	bool ReadPZMFile(PZMFile* PZM_file, const std::string& file_name);
}