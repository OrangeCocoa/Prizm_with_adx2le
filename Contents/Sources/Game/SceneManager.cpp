
#include"SceneManager.h"
#include"Scenes\BaseScene.h"
#include"Scenes\MainGameScene.h"

namespace Prizm
{
	class SceneManager::Impl
	{
	public:
		std::unique_ptr<BaseScene> cur_scene_;

		Impl(void){}
	};

	SceneManager::SceneManager(void) : impl_(std::make_unique<Impl>())
	{
		impl_->cur_scene_ = std::make_unique<BaseScene>();
	}

	SceneManager::~SceneManager(void) = default;

	void SceneManager::Update(void)
	{
		impl_->cur_scene_->Update();
	}

	void SceneManager::Draw(void)
	{
		impl_->cur_scene_->Draw();
	}

	void SceneManager::Finalize(void)
	{
		impl_->cur_scene_->Exit();
	}
}