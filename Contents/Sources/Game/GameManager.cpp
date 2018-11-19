
#include"GameManager.h"
#include"..\Graphics\Graphics.h"
#include"..\Window\Window.h"
#include"..\Framework\Log.h"
#include"..\Input\Input.h"
#include"SceneManager.h"
#include"Scenes\TitleScene.h"
#include"Scenes\MainGameScene.h"
#include"Scenes\GameOverScene.h"

namespace Prizm
{
	class GameManager::Impl
	{
	public:
		bool want_exit_;
		std::unique_ptr<SceneManager> scene_manager_;

		Impl() :
			want_exit_(false){}
	};

	GameManager::GameManager() : impl_(std::make_unique<Impl>()){}

	GameManager::~GameManager() = default;

	bool GameManager::Initialize(HWND window_handle)
	{
		if (!Graphics::Initialize(window_width<int>, window_height<int>, false, window_handle, false))
			return false;

		impl_->scene_manager_ = std::make_unique<SceneManager>();

		impl_->scene_manager_->SetNextScene<MainGameScene>();

		return true;
	}

	bool GameManager::Run(void)
	{
		if (Input::IsKeyTriggered("F9"))
		{// input test
			Graphics::ChangeWindowMode();
			//impl_->want_exit_ = true;
		}

		// scene update
		impl_->scene_manager_->Update();

		// scene draw
		Graphics::BeginFrame();
		impl_->scene_manager_->Draw();
		Graphics::EndFrame();

		return impl_->want_exit_;
	}

	void GameManager::Finalize(void)
	{
		impl_->scene_manager_->Finalize();
		Graphics::Finalize();
	}
}