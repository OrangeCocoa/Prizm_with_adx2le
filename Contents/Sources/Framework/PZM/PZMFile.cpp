
#include<iostream>
#include<fstream>
#include<filesystem>

#include"PZMFile.h"
#include"..\Log.h"
#include"..\Resource.h"

namespace Prizm
{
	const std::string MODEL_DIR = RESOURCE_DIR + "Model\\";

	template <typename Type>
	void Read(Type* value, std::ifstream& file)
	{
		file.read(reinterpret_cast<char*>(value), sizeof(*value));
	}

	template <typename Type>
	void Read(Type* buffer, unsigned int size, std::ifstream& file)
	{// string read
		file.read(reinterpret_cast<char*>(buffer), size);
	}

	bool ReadString(PZMFile* pzm_file, std::string* value, std::ifstream& file)
	{
		int buffer_size;
		Read(&buffer_size, file);

		if (buffer_size > 0)
		{
			std::string str(buffer_size, '\0');
			Read(&str[0], buffer_size, file);
			*value = str;

			return true;
		}
		else return false;
	}

	void PZMReadIndex(int* out_idx, unsigned int size, std::ifstream& file)
	{// pzm index is 1 or 2 or 4
		switch (size)
		{
		case 1:
		{
			char idx;
			Read(&idx, file);
			if (idx < 0xFF)
			{
				*out_idx = static_cast<int>(idx);
			}
			else
			{
				*out_idx = -1;
			}
		}
		return;
		case 2:
		{
			short idx;
			Read(&idx, file);
			if (idx < 0xFFFF)
			{
				*out_idx = static_cast<int>(idx);
			}
			else
			{
				*out_idx = -1;
			}
		}
		return;
		case 4:
		{
			int idx;
			Read(&idx, file);
			if (idx < 0xFF)
			{
				*out_idx = static_cast<int>(idx);
			}
			else
			{
				*out_idx = -1;
			}
		}
		return;
		}
	}

	bool ReadHeader(PZMFile* pzm_file, std::ifstream& file)
	{
		auto& header = pzm_file->header;

		Read(&header.data_size, file);

		Read(&header.add_UV_num, file);
		Read(&header.vertex_index_size, file);
		Read(&header.texture_index_size, file);
		Read(&header.material_index_size, file);
		Read(&header.bone_index_size, file);
		Read(&header.morph_index_size, file);
		Read(&header.rigidbody_index_size, file);

		return true;
	}

	bool ReadInfo(PZMFile* pzm_file, std::ifstream& file)
	{
		auto& info = pzm_file->info;
		ReadString(pzm_file, &info.model_name, file);
		ReadString(pzm_file, &info.comment, file);

		return true;
	}

	bool ReadVertex(PZMFile* pzm_file, std::ifstream& file)
	{
		int vertex_count;
		Read(&vertex_count, file);

		auto& vertices = pzm_file->vertices;
		vertices.resize(vertex_count);

		for (int i = 0; i < vertex_count; ++i)
		{
			Read(&vertices[i].position, file);
			Read(&vertices[i].normal, file);
			Read(&vertices[i].uv, file);

			for (int i = 0; i < pzm_file->header.add_UV_num; ++i)
			{
				Read(&vertices[i].add_UV[i], file);
			}

			Read(&vertices[i].weight_type, file);

			switch (vertices[i].weight_type)
			{
			case PZMVertex::VertexWeight::BDEF1:
				Read(&vertices[i].bone_indices[0], file);
				break;
			case PZMVertex::VertexWeight::BDEF2:
				Read(&vertices[i].bone_indices[0], file);
				Read(&vertices[i].bone_indices[1], file);
				Read(&vertices[i].bone_weights[0], file);
				break;
			case PZMVertex::VertexWeight::BDEF4:
			case PZMVertex::VertexWeight::QDEF: // same BDEF4
				for (int i = 0; i < 4; ++i)
				{
					Read(&vertices[i].bone_indices[i], file);
					Read(&vertices[i].bone_weights[i], file);
				}
				break;
			case PZMVertex::VertexWeight::SDEF:
				Read(&vertices[i].bone_indices[0], file);
				Read(&vertices[i].bone_indices[1], file);
				Read(&vertices[i].bone_weights[0], file);
				Read(&vertices[i].sdef_C, file);
				Read(&vertices[i].sdef_R0, file);
				Read(&vertices[i].sdef_R1, file);
				break;
			}

			Read(&vertices[i].edge_mag, file);
		}

		return true;
	}

	bool ReadFace(PZMFile* pzm_file, std::ifstream& file)
	{
		int face_count;
		Read(&face_count, file);

		auto& faces = pzm_file->faces;
		faces.resize(face_count);

		for (int i = 0; i < face_count; ++i)
		{
			Read(&faces[i].indices[0], file);
			Read(&faces[i].indices[1], file);
			Read(&faces[i].indices[2], file);
		}

		return true;
	}

	bool ReadTexture(PZMFile* pzm_file, std::ifstream& file)
	{
		int texture_count;
		Read(&texture_count, file);

		auto& textures = pzm_file->textures;
		textures.resize(texture_count);

		for (int i = 0; i < texture_count; ++i)
		{
			ReadString(pzm_file, &textures[i].texture_name, file);
		}

		return true;
	}

	bool ReadMaterial(PZMFile* pzm_file, std::ifstream& file)
	{
		int material_count;
		Read(&material_count, file);

		auto& materials = pzm_file->materials;
		materials.resize(material_count);

		for (int i = 0; i < material_count; ++i)
		{
			ReadString(pzm_file, &materials[i].name, file);

			Read(&materials[i].diffuse, file);
			Read(&materials[i].specular, file);
			Read(&materials[i].specular_power, file);
			Read(&materials[i].ambient, file);
			Read(&materials[i].draw_mode, file);
			Read(&materials[i].edge_color, file);
			Read(&materials[i].edge_size, file);

			Read(&materials[i].texture_index, file);
			Read(&materials[i].sphere_texture_index, file);
			Read(&materials[i].sphere_mode, file);
			Read(&materials[i].toon_mode, file);

			switch (materials[i].toon_mode)
			{
			case PZMMaterial::ToonMode::Separate:
				Read(&materials[i].toon_texture_index, file);
				break;
			case PZMMaterial::ToonMode::Common:
				uint8_t common_idx;
				Read(&common_idx, file);
				materials[i].toon_texture_index = common_idx;
				break;
			}

			Read(&materials[i].num_face_vertices, file);
		}

		return true;
	}

	bool ReadBone(PZMFile* pzm_file, std::ifstream& file)
	{
		int bone_count;
		Read(&bone_count, file);

		auto& bones = pzm_file->bones;
		bones.resize(bone_count);

		for (int i = 0; i < bone_count; ++i)
		{
			ReadString(pzm_file, &bones[i].name, file);

			Read(&bones[i].position, file);
			Read(&bones[i].parent_bone_index, file);
			Read(&bones[i].deform_depth, file);

			Read(&bones[i].bone_flag, file);

			if ((static_cast<unsigned short>(bones[i].bone_flag) &
				static_cast<unsigned short>(PZMBone::BoneFlags::TargetShowMode)) == 0)
			{
				Read(&bones[i].position_offset, file);
			}
			else
			{
				Read(&bones[i].link_bone_index, file);
			}

			if ((static_cast<unsigned short>(bones[i].bone_flag) &
				static_cast<unsigned short>(PZMBone::BoneFlags::AppendRotate)) != 0 ||
				(static_cast<unsigned short>(bones[i].bone_flag) &
					static_cast<unsigned short>(PZMBone::BoneFlags::AppendTranslate)) != 0)
			{
				Read(&bones[i].append_bone_index, file);
				Read(&bones[i].append_weight, file);
			}

			if ((static_cast<unsigned short>(bones[i].bone_flag) &
				static_cast<unsigned short>(PZMBone::BoneFlags::FixedAxis)) != 0)
			{
				Read(&bones[i].fixed_axis, file);
			}

			if ((static_cast<unsigned short>(bones[i].bone_flag) &
				static_cast<unsigned short>(PZMBone::BoneFlags::LocalAxis)) != 0)
			{
				Read(&bones[i].local_X_axis, file);
				Read(&bones[i].local_Z_axis, file);
			}

			if ((static_cast<unsigned short>(bones[i].bone_flag) &
				static_cast<unsigned short>(PZMBone::BoneFlags::DeformOuterParent)) != 0)
			{
				Read(&bones[i].key_value, file);
			}

			if ((static_cast<unsigned short>(bones[i].bone_flag) &
				static_cast<unsigned short>(PZMBone::BoneFlags::IK)) != 0)
			{
				Read(&bones[i].ik_target_bone_index, file);
				Read(&bones[i].ik_iteration_count, file);
				Read(&bones[i].ik_limit, file);

				int ik_link_count;
				Read(&ik_link_count, file);

				auto& ik_links = bones[i].ik_links;
				ik_links.resize(ik_link_count);

				for (auto& ik_link : ik_links)
				{
					Read(&ik_link.ik_bone_index, file);
					Read(&ik_link.enable_limit, file);

					if (ik_link.enable_limit == 1)
					{
						Read(&ik_link.limit_min, file);
						Read(&ik_link.limit_max, file);
					}
				}
			}
		}

		return true;
	}

	bool ReadMorph(PZMFile* pzm_file, std::ifstream& file)
	{
		int morph_count;
		Read(&morph_count, file);

		auto& morphs = pzm_file->morphs;
		morphs.resize(morph_count);

		for (int i = 0; i < morph_count; ++i)
		{
			ReadString(pzm_file, &morphs[i].name, file);

			Read(&morphs[i].control_panel, file);
			Read(&morphs[i].morph_type, file);

			int offset_count;
			Read(&offset_count, file);

			switch (morphs[i].morph_type)
			{
			case PZMMorph::MorphType::Position:
			{
				auto& pos_morphs = morphs[i].position_morph;
				pos_morphs.resize(offset_count);

				for (auto& pm : pos_morphs)
				{
					Read(&pm.vertex_index, file);
					Read(&pm.position, file);
				}

				break;
			}
			case PZMMorph::MorphType::UV:
			{
				auto& uv_morphs = morphs[i].uv_morph;
				uv_morphs.resize(offset_count);

				for (auto& um : uv_morphs)
				{
					Read(&um.vertex_index, file);
					Read(&um.uv, file);
				}

				break;
			}
			case PZMMorph::MorphType::Bone:
			{
				auto& bone_morphs = morphs[i].bone_morph;
				bone_morphs.resize(offset_count);

				for (auto& bm : bone_morphs)
				{
					Read(&bm.bone_index, file);
					Read(&bm.position, file);
					Read(&bm.quaternion, file);
				}

				break;
			}
			case PZMMorph::MorphType::Material:
			{
				auto& mat_morphs = morphs[i].material_morph;
				mat_morphs.resize(offset_count);

				for (auto& mm : mat_morphs)
				{
					Read(&mm.material_index, file);
					Read(&mm.op_type, file);
					Read(&mm.diffuse, file);
					Read(&mm.specular, file);
					Read(&mm.specular_power, file);
					Read(&mm.ambient, file);
					Read(&mm.edge_color, file);
					Read(&mm.edge_size, file);
					Read(&mm.texture_factor, file);
					Read(&mm.sphere_texture_factor, file);
					Read(&mm.toon_texture_factor, file);
				}

				break;
			}
			case PZMMorph::MorphType::Group:
			{
				auto& group_morphs = morphs[i].group_morph;
				group_morphs.resize(offset_count);

				for (auto& gm : group_morphs)
				{
					Read(&gm.morph_index, file);
					Read(&gm.weight, file);
				}

				break;
			}
			case PZMMorph::MorphType::Flip:
			{
				auto& flip_morphs = morphs[i].flip_morph;
				flip_morphs.resize(offset_count);

				for (auto& fm : flip_morphs)
				{
					Read(&fm.morph_index, file);
					Read(&fm.weight, file);
				}

				break;
			}
			case PZMMorph::MorphType::Impulse:
			{
				auto& impulse_morphs = morphs[i].impulse_morph;
				impulse_morphs.resize(offset_count);

				for (auto& im : impulse_morphs)
				{
					Read(&im.rigidbody_index, file);
					Read(&im.local_flag, file);
					Read(&im.translate_velocity, file);
					Read(&im.rotate_torque, file);
				}

				break;
			}
			}
		}

		return true;
	}

	bool ReadRigidbody(PZMFile* pzm_file, std::ifstream& file)
	{
		int rb_count;
		Read(&rb_count, file);

		auto& rigidbodies = pzm_file->rigidbodies;
		rigidbodies.resize(rb_count);

		for (int i = 0; i < rb_count; ++i)
		{
			ReadString(pzm_file, &rigidbodies[i].name, file);

			Read(&rigidbodies[i].bone_index, file);

			Read(&rigidbodies[i].group, file);
			Read(&rigidbodies[i].collision_group, file);

			Read(&rigidbodies[i].shape, file);
			Read(&rigidbodies[i].shape_size, file);

			Read(&rigidbodies[i].translate, file);
			Read(&rigidbodies[i].rotate, file);

			Read(&rigidbodies[i].mass, file);
			Read(&rigidbodies[i].translate_dimmer, file);
			Read(&rigidbodies[i].rotate_dimmer, file);
			Read(&rigidbodies[i].repulsion, file);
			Read(&rigidbodies[i].friction, file);
			Read(&rigidbodies[i].op, file);
		}

		return true;
	}

	bool ReadJoint(PZMFile* pzm_file, std::ifstream& file)
	{
		int joint_count;
		Read(&joint_count, file);

		auto& joints = pzm_file->joints;
		joints.resize(joint_count);

		for (auto& joint : joints)
		{
			ReadString(pzm_file, &joint.name, file);

			Read(&joint.type, file);
			Read(&joint.rigidbody_A_index, file);
			Read(&joint.rigidbody_B_index, file);

			Read(&joint.translate, file);
			Read(&joint.rotate, file);
			Read(&joint.translate_lower_limit, file);
			Read(&joint.translate_upper_limit, file);
			Read(&joint.rotate_lower_limit, file);
			Read(&joint.rotate_upper_limit, file);

			Read(&joint.spring_translate_factor, file);
			Read(&joint.spring_rotate_factor, file);
		}

		return true;
	}

	bool ReadSoftbody(PZMFile* pzm_file, std::ifstream& file)
	{
		int sb_count;
		Read(&sb_count, file);

		auto& softbodies = pzm_file->softbodies;
		softbodies.resize(sb_count);

		for (int i = 0; i < sb_count; ++i)
		{
			ReadString(pzm_file, &softbodies[i].name, file);

			Read(&softbodies[i].type, file);
			Read(&softbodies[i].material_index, file);

			Read(&softbodies[i].group, file);
			Read(&softbodies[i].collision_group, file);

			Read(&softbodies[i].flag, file);
			Read(&softbodies[i].B_link_length, file);
			Read(&softbodies[i].num_clusters, file);
			Read(&softbodies[i].total_mass, file);
			Read(&softbodies[i].collision_margin, file);

			Read(&softbodies[i].aero_model, file);

			// config
			Read(&softbodies[i].VCF, file);
			Read(&softbodies[i].DP, file);
			Read(&softbodies[i].DG, file);
			Read(&softbodies[i].LF, file);
			Read(&softbodies[i].PR, file);
			Read(&softbodies[i].VC, file);
			Read(&softbodies[i].DF, file);
			Read(&softbodies[i].MT, file);
			Read(&softbodies[i].CHR, file);
			Read(&softbodies[i].KHR, file);
			Read(&softbodies[i].SHR, file);
			Read(&softbodies[i].AHR, file);

			// cluster
			Read(&softbodies[i].SRHR_CL, file);
			Read(&softbodies[i].SKHR_CL, file);
			Read(&softbodies[i].SSHR_CL, file);
			Read(&softbodies[i].SR_SPLT_CL, file);
			Read(&softbodies[i].SK_SPLT_CL, file);
			Read(&softbodies[i].SS_SPLT_CL, file);

			//interation
			Read(&softbodies[i].V_IT, file);
			Read(&softbodies[i].P_IT, file);
			Read(&softbodies[i].D_IT, file);
			Read(&softbodies[i].C_IT, file);

			//material
			Read(&softbodies[i].LST, file);
			Read(&softbodies[i].AST, file);
			Read(&softbodies[i].VST, file);

			int anchor_count;
			Read(&anchor_count, file);
			softbodies[i].anchor_rigidbodies.resize(anchor_count);

			for (auto& anchor : softbodies[i].anchor_rigidbodies)
			{
				Read(&anchor.rigid_body_index, file);
				Read(&anchor.vertex_index, file);
				Read(&anchor.near_mode, file);
			}

			int pin_count;
			Read(&pin_count, file);
			softbodies[i].pin_vertex_indices.resize(pin_count);

			for (auto& pin : softbodies[i].pin_vertex_indices)
			{
				Read(&pin, file);
			}
		}

		return true;
	}

	bool ReadPZMFile(PZMFile* pzm_file, const std::string& file_name)
	{
		std::string file_path = MODEL_DIR + file_name;

		std::ifstream file;
		file.open(file_path, std::ios::in | std::ios::binary);

		if (!file)
		{
			Log::Error("ファイル名 : [" + file_path + "] が開けません");
			return false;
		}

		// binary size calculate
		file.seekg(0, std::ios::end);
		std::streampos binary_size = file.tellg();
		file.clear();
		file.seekg(0, std::ios::beg);

		if (!ReadHeader(pzm_file, file)) return false;
		if (!ReadInfo(pzm_file, file)) return false;
		if (!ReadVertex(pzm_file, file)) return false;
		if (!ReadFace(pzm_file, file)) return false;
		if (!ReadTexture(pzm_file, file)) return false;
		if (!ReadMaterial(pzm_file, file)) return false;
		if (!ReadBone(pzm_file, file)) return false;
		if (!ReadMorph(pzm_file, file)) return false;
		if (!ReadRigidbody(pzm_file, file)) return false;
		if (!ReadJoint(pzm_file, file)) return false;

		if (file.tellg() != binary_size)
		{// soft body included only ver 2.1 read
			if (!ReadSoftbody(pzm_file, file)) return false;
		}

		file.close();

		return true;
	}
}