
#include"GameManager.h"
#include"..\Graphics\Graphics.h"
#include"..\Window\Window.h"
#include"..\Framework\Log.h"
#include"..\Input\Input.h"
#include"SceneManager.h"
#include"Scenes\TitleScene.h"
#include"Scenes\MainGameScene.h"
#include"Scenes\GameOverScene.h"
#include"..\Framework\Adx2\Adx2Manager.h"

namespace Prizm
{
	class GameManager::Impl
	{
	public:
		bool want_exit_;
		std::unique_ptr<SceneManager> scene_manager_;

		Impl() : want_exit_(false){}
	};

	GameManager::GameManager() : impl_(std::make_unique<Impl>()){}

	GameManager::~GameManager() = default;

	bool GameManager::Initialize(HWND window_handle)
	{
		if (!Graphics::Initialize(window_width<int>, window_height<int>, false, window_handle, false))
			return false;

		Adx2Manager::Initialize();
		Adx2Manager::LoadAcb("GeneralCueSheet.acb", "GeneralCueSheet.awb", Adx2Manager::SceneAcb::GENERAL);

		impl_->scene_manager_ = std::make_unique<SceneManager>();
		impl_->scene_manager_->SetNextScene<TitleScene>();

		return true;
	}

	bool GameManager::Run(void)
	{
		if (Input::IsKeyTriggered("F9"))
		{// input test
			//Graphics::ChangeWindowMode();
		}

		if (Input::IsKeyPress("Up"))
			Adx2Manager::UpPitchAll();

		if (Input::IsKeyPress("Down"))
			Adx2Manager::DownPitchAll();

		if (Input::IsKeyPress("Enter"))
			Adx2Manager::RestorePitchAll();

		if (Input::IsKeyTriggered("F3"))
		{
			static int pause_status = 0;
			Adx2Manager::Pause(pause_status ^= 1);
		}

		// scene update
		if (impl_->scene_manager_->Update())
		{
			// scene draw
			Graphics::BeginFrame();
			impl_->scene_manager_->Draw();
			Graphics::EndFrame();
		}

		Adx2Manager::Update();

		return impl_->want_exit_;
	}

	void GameManager::Finalize(void)
	{
		Adx2Manager::Finalize();
		impl_->scene_manager_->Finalize();
		Graphics::Finalize();
	}
}