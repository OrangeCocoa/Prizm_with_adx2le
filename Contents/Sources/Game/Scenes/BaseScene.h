#pragma once

#include<memory>
#include<vector>
#include<unordered_map>
#include<string>

#include"..\..\Framework\Entity.h"
#include"..\..\Framework\ResourcePool.h"
#include"..\..\Graphics\Shader.h"
#include"..\..\Graphics\Texture.h"
#include"..\..\Graphics\Geometry.h"
#include"..\..\Graphics\GeometryGenerator.h"
#include"..\..\Window\Window.h"

namespace Prizm
{
	class BaseScene
	{
	private:
		std::unordered_map<std::string, ResourcePool<Entity>> game_objects_;
		ResourcePool<Shader> shaders_;
		ResourcePool<Texture> textures_;

		struct FadeResource
		{
			std::unique_ptr<Geometry> screen_quad;
			std::weak_ptr<Shader> quad_shader_vs;
			std::weak_ptr<Shader> quad_shader_ps;
		};

		FadeResource fade_resource_;

	protected:
		std::unordered_map<std::string, std::vector<unsigned int>> game_object_indices_;
		int score_;
		unsigned int ui_shader_vs_index_;
		unsigned int ui_shader_ps_index_;

		void FadeIn()
		{

		}

		void FadeOut()
		{

		}

		const std::shared_ptr<Shader>& GetShader(unsigned int index)
		{
			return shaders_.Get(index);
		}

		const std::shared_ptr<Texture>& GetTexture(unsigned int index)
		{
			return textures_.Get(index);
		}

		void ReleaseShader(unsigned int index)
		{
			shaders_.Release(index);
		}

		void ReleaseTexture(unsigned int index)
		{
			textures_.Release(index);
		}

	public:
		BaseScene(void);
		virtual ~BaseScene(void) = default;
		virtual void LoadScene(void) {}
		virtual void Update(void) {}
		virtual void Draw(void) {}
		virtual void Finalize(void) {}

		template<class _Type>
		void AddGameObject(void)
		{
			auto game_object_index = game_objects_[typeid(_Type).name()].Load(std::make_shared<_Type>());
			game_object_indices_[typeid(_Type).name()].emplace_back(game_object_index);
			game_objects_[typeid(_Type).name()].Get(game_object_index)->Initialize();
		}
		
		template<class _Type>
		std::shared_ptr<_Type> GetGameObject(unsigned int index)
		{
			auto game_object = game_objects_[typeid(_Type).name()].Get(index);
			return std::static_pointer_cast<_Type>(game_object);
		}

		// all game objects function
		void RunEntities(void);
		void DrawEntities(void);
		void FinalizeEntities(void);
	};
}