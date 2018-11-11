
#include<unordered_map>
#include"BaseScene.h"
#include"..\..\Graphics\Graphics.h"
#include"..\..\Game\Camera.h"
#include"..\..\Framework\Entity.h"
#include"..\..\Window\Window.h"

namespace Prizm
{
	BaseScene::BaseScene(void)
	{
		ui_shader_vs_index_ = shaders_.Load(std::make_shared<Shader>("2D.hlsl"));
		ui_shader_ps_index_ = shaders_.Load(std::make_shared<Shader>("2D.hlsl"));

		std::vector<D3D11_INPUT_ELEMENT_DESC> def_element =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		fade_resource_.quad_shader_vs = shaders_.Get(ui_shader_vs_index_);
		fade_resource_.quad_shader_ps = shaders_.Get(ui_shader_ps_index_);
		fade_resource_.quad_shader_vs.lock()->CompileAndCreateFromFile(Graphics::GetDevice(), ShaderType::VS, def_element);
		fade_resource_.quad_shader_ps.lock()->CompileAndCreateFromFile(Graphics::GetDevice(), ShaderType::PS, def_element);
		
		float width = window_width<float>;
		float height = window_height<float>;
		fade_resource_.screen_quad = std::make_unique<Geometry>(GeometryGenerator::Quad2D(width, height, width / 2, height / 2));
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