#pragma once

#include<memory>
#include<vector>
#include<unordered_map>
#include<string>

#include"..\..\Framework\Entity.h"
#include"..\..\Framework\ResourcePool.h"
#include"..\..\Framework\Shader.h"
#include"..\..\Framework\Texture.h"
#include"..\..\Graphics\Geometry.h"

namespace Prizm
{
	class SceneManager;

	class BaseScene
	{
	private:
		ResourcePool<Entity> back_ground_;
		std::unordered_map<std::string, ResourcePool<Entity>> game_objects_2D_;		// UI
		std::unordered_map<std::string, ResourcePool<Entity>> game_objects_3D_;		// objects

		ResourcePool<Shader> shaders_;
		ResourcePool<Texture> textures_;

		std::unique_ptr<Geometry> screen_quad_;
		

	protected:
		static SceneManager* scene_manager_;
		std::unordered_map<std::string, std::vector<unsigned int>> game_object_indices_;
		int score_;

		unsigned int quad_shader_;
		unsigned int object_shader_;
		unsigned int shadow_map_shader_;
		unsigned int hdao_shader_;

		SceneManager* GetSceneManager(void) { return scene_manager_; }

		void FadeIn(unsigned int);

		void FadeOut(unsigned int);

		unsigned int CreateShader(const std::string&);

		void CompileShader(const unsigned int, const ShaderType, const std::vector<D3D11_INPUT_ELEMENT_DESC>&);

		unsigned int LoadTexture(const std::string&);

		const std::shared_ptr<Shader>& GetShader(const unsigned int);

		const std::shared_ptr<Texture>& GetTexture(const unsigned int);

		void ReleaseShader(const unsigned int);

		void ReleaseTexture(const unsigned int);

	public:
		BaseScene(void);
		virtual ~BaseScene(void) = default;
		virtual void LoadScene(void) {}
		virtual bool Update(void) { return true; }
		virtual void Draw(void) {}
		virtual void Finalize(void) {}

		template<class _Type>
		unsigned int AddBackGround(void)
		{
			auto game_object_index = back_ground_.Load(std::make_shared<_Type>());
			game_object_indices_[typeid(_Type).name()].emplace_back(game_object_index);
			back_ground_.Get(game_object_index)->Initialize();
			return game_object_index;
		}

		template<class _Type>
		std::shared_ptr<_Type> GetBackGround(unsigned int index)
		{
			auto game_object = back_ground_.Get(index);
			return std::static_pointer_cast<_Type>(game_object);
		}

		template<class _Type>
		unsigned int AddGameObject2D(void)
		{
			auto game_object_index = game_objects_2D_[typeid(_Type).name()].Load(std::make_shared<_Type>());
			game_object_indices_[typeid(_Type).name()].emplace_back(game_object_index);
			game_objects_2D_[typeid(_Type).name()].Get(game_object_index)->Initialize();
			return game_object_index;
		}
		
		template<class _Type>
		std::shared_ptr<_Type> GetGameObject2D(unsigned int index)
		{
			auto game_object = game_objects_2D_[typeid(_Type).name()].Get(index);
			return std::static_pointer_cast<_Type>(game_object);
		}

		template<class _Type>
		unsigned int AddGameObject3D(void)
		{
			auto game_object_index = game_objects_3D_[typeid(_Type).name()].Load(std::make_shared<_Type>());
			game_object_indices_[typeid(_Type).name()].emplace_back(game_object_index);
			game_objects_3D_[typeid(_Type).name()].Get(game_object_index)->Initialize();
			return game_object_index;
		}

		template<class _Type>
		std::shared_ptr<_Type> GetGameObject3D(unsigned int index)
		{
			auto game_object = game_objects_3D_[typeid(_Type).name()].Get(index);
			return std::static_pointer_cast<_Type>(game_object);
		}

		// all game objects function
		void RunEntities(void);
		void DrawEntities(void);
		void FinalizeEntities(void);

		static void SetSceneManager(SceneManager* sm);
	};
}