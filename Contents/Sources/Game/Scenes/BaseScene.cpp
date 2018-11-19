
#include<unordered_map>
#include"BaseScene.h"
#include"..\..\Graphics\Graphics.h"
#include"..\..\Graphics\GeometryGenerator.h"
#include"..\..\Game\Camera.h"
#include"..\..\Window\Window.h"

namespace Prizm
{
	BaseScene::BaseScene(void)
	{
		/*for (unsigned int i = 0; i < ShaderFile::SHADER_MAX; ++i)
		{

		}*/

		// 2D shader
		quad_shader_ = CreateShader("2D.hlsl");

		std::vector<D3D11_INPUT_ELEMENT_DESC> ui_element =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		CompileShader(quad_shader_, ShaderType::VS, ui_element);
		CompileShader(quad_shader_, ShaderType::PS, ui_element);

		fade_resource_.screen_quad = std::make_unique<Geometry>(GeometryGenerator::Quad2D(window_width<float>, window_height<float>, 0, 0));
		
		// shadow map object
		object_shader_ = CreateShader("Texture.hlsl");
		//object_shader_ = CreateShader("Object.hlsl");

		std::vector<D3D11_INPUT_ELEMENT_DESC> object_element =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, 52, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		CompileShader(object_shader_, ShaderType::VS, object_element);
		CompileShader(object_shader_, ShaderType::PS, object_element);

		shadow_map_shader_ = CreateShader("ShadowMap.hlsl");
	}

	void BaseScene::FadeIn(unsigned int curr_time)
	{

	}

	void BaseScene::FadeOut(unsigned int curr_time)
	{

	}

	unsigned int BaseScene::CreateShader(const std::string& file_name)
	{
		return shaders_.Load(std::make_shared<Shader>(file_name));
	}

	void BaseScene::CompileShader(const unsigned int index, const ShaderType type, const std::vector<D3D11_INPUT_ELEMENT_DESC>& element_desc)
	{
		shaders_.Get(index)->CompileAndCreateFromFile(Graphics::GetDevice(), type, element_desc);
	}

	unsigned int BaseScene::LoadTexture(const std::string& tex_name)
	{
		auto index = textures_.Load(std::make_shared<Texture>());
		textures_.Get(index)->LoadTexture(tex_name);
		return index;
	}

	const std::shared_ptr<Shader>& BaseScene::GetShader(const unsigned int index)
	{
		return shaders_.Get(index);
	}

	const std::shared_ptr<Texture>& BaseScene::GetTexture(const unsigned int index)
	{
		return textures_.Get(index);
	}

	void BaseScene::ReleaseShader(const unsigned int index)
	{
		shaders_.Release(index);
	}

	void BaseScene::ReleaseTexture(const unsigned int index)
	{
		textures_.Release(index);
	}

	void BaseScene::RunEntities(void)
	{
		for (unsigned int i = 0; i < back_ground_.Size(); ++i)
		{
			if (back_ground_.Get(i))
				back_ground_.Get(i)->Run();
		}

		for (auto&& game_object : game_objects_3D_)
		{
			for (auto index : game_object_indices_[game_object.first])
			{
				if (game_object.second.Get(index))
					game_object.second.Get(index)->Run();
			}
		}

		for (auto&& game_object : game_objects_2D_)
		{
			for (auto index : game_object_indices_[game_object.first])
			{
				if (game_object.second.Get(index))
					game_object.second.Get(index)->Run();
			}
		}
	}

	void BaseScene::DrawEntities(void)
	{
		for (unsigned int i = 0; i < back_ground_.Size(); ++i)
		{
			if (back_ground_.Get(i))
				back_ground_.Get(i)->Draw();
		}

		for (auto&& game_object : game_objects_3D_)
		{
			for (auto index : game_object_indices_[game_object.first])
			{
				if (game_object.second.Get(index))
					game_object.second.Get(index)->Draw();
			}
		}

		for (auto&& game_object : game_objects_2D_)
		{
			for (auto index : game_object_indices_[game_object.first])
			{
				if (game_object.second.Get(index))
					game_object.second.Get(index)->Draw();
			}
		}
	}

	void BaseScene::FinalizeEntities(void)
	{
		for (unsigned int i = 0; i < back_ground_.Size(); ++i)
		{
			if (back_ground_.Get(i))
			{
				back_ground_.Get(i)->Finalize();
				back_ground_.Release(i);
			}
		}

		for (auto&& game_object : game_objects_3D_)
		{
			for (auto index : game_object_indices_[game_object.first])
			{
				if (game_object.second.Get(index))
				{
					game_object.second.Get(index)->Finalize();
					game_object.second.Release(index);
				}
			}
		}

		for (auto&& game_object : game_objects_2D_)
		{
			for (auto index : game_object_indices_[game_object.first])
			{
				if (game_object.second.Get(index))
				{
					game_object.second.Get(index)->Finalize();
					game_object.second.Release(index);
				}
			}
		}
	}
}