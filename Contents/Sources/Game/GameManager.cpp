
#include"GameManager.h"
#include"..\Graphics\Graphics.h"
#include"..\Window\Window.h"
#include"..\Framework\Log.h"
#include"..\Input\Input.h"
//#include"Scenes\MainGameScene.h"
//#include"SceneManager.h"

namespace Prizm
{
	class GameManager::Impl
	{
	public:
		bool want_exit_;
		std::unique_ptr<Graphics> graphics_;
		//std::unique_ptr<SceneManager> scene_manager_;

		Impl() :
			graphics_(std::make_unique<Graphics>()),
			//scene_manager_(std::make_unique<SceneManager>(graphics_)),
			want_exit_(false){}
	};

	GameManager::GameManager() : impl_(std::make_unique<Impl>()){}

	GameManager::~GameManager() = default;

	bool GameManager::Init(HWND window_handle)
	{
		if (!impl_->graphics_->Init(window_width<int>, window_height<int>, false, window_handle, false))
			return false;

		//impl_->scene_manager_->SetNextScene<MainGameScene>();

		return true;
	}

	bool GameManager::Run(void)
	{
		if (Input::IsKeyTriggered("F9"))
		{// input test
			impl_->graphics_->ChangeWindowMode();
			//impl_->want_exit_ = true;
		}

		// scene update
		//impl_->scene_manager_->Update();

		// scene draw
		impl_->graphics_->BeginFrame();
		//impl_->scene_manager_->Draw();
		impl_->graphics_->EndFrame();

		return impl_->want_exit_;
	}

	void GameManager::Exit(void)
	{
		//impl_->scene_manager_->Exit();
		impl_->graphics_->Exit();
	}
}