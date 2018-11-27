#pragma once

#include<memory>

#include"Scenes\BaseScene.h"
#include"Scenes\MainGameScene.h"
#include"..\Framework\Log.h"

namespace Prizm
{
	class SceneManager
	{
	private:
		std::unique_ptr<BaseScene> cur_scene_;

	public:
		SceneManager(void)
		{
			cur_scene_ = std::make_unique<BaseScene>();
			cur_scene_->SetSceneManager(this);
		}

		~SceneManager(void) = default;

		template<class SceneTypes>
		void SetNextScene(void)
		{
			if (cur_scene_)
			{
				cur_scene_->Finalize();
				cur_scene_.reset();
			}
			
			cur_scene_ = std::make_unique<SceneTypes>();
			cur_scene_->LoadScene();

			Log::Info("Scene changed.");
		}

		bool Update(void)
		{
			return cur_scene_->Update();
		}

		void Draw(void)
		{
			cur_scene_->Draw();
		}

		void Finalize(void)
		{
			cur_scene_->Finalize();
		}
	};
}