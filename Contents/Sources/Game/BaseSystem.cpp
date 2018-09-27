
#include <windows.h>

#include"BaseSystem.h"
#include"GameManager.h"
#include"..\Framework\Utils.h"
#include"..\Framework\Log.h"
#include"..\Input\Input.h"
#include"..\Window\Window.h"

#pragma comment(lib, "Input.lib")
#pragma comment(lib, "Framework.lib")
#pragma comment(lib, "Window.lib")
#pragma comment(lib, "Graphics.lib")

namespace Prizm
{
	std::string BaseSystem::workspace_directory_ = "";

	class BaseSystem::Impl
	{
	public:
		bool app_exit_;
		std::unique_ptr<Window> window_;
		std::shared_ptr<GameManager> game_manager_;

		Impl() 
			: app_exit_(false)
			, window_(std::make_unique<Window>())
			, game_manager_(std::make_shared<GameManager>())
		{}

		void MessageLoop(void)
		{
			MSG msg = {};

			while (!app_exit_)
			{
				if (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessageA(&msg);
				}

				if (Input::IsKeyTriggered("escape"))
				{
					if (Input::IsMouseCaptured())
					{
						Input::CaptureMouse(window_->GetWindowHandle(), false);
					}
					else
					{
						if (MessageBoxA(window_->GetWindowHandle(), "Quit ?", "User Notification", MB_YESNO | MB_DEFBUTTON2) == IDYES)
						{
							Log::Info("[EXIT] KEY DOWN ESC");
							app_exit_ = true;
						}
					}
				}

				if (msg.message == WM_QUIT)
				{
					app_exit_ = true;
				}
				else
				{
					app_exit_ |= game_manager_->Run();
				}

				Input::PostStateUpdate();
			}
		}
	};

	BaseSystem::BaseSystem() : impl_(std::make_unique<Impl>()) {}

	BaseSystem::~BaseSystem() { Log::Info("~BaseSystem()"); }// = default;

	bool BaseSystem::Initialize(void)
	{
		workspace_directory_ = DirectoryUtils::GetSpecialFolderPath(DirectoryUtils::FolderType::APPDATA) + "\\PrizmEngine";

#ifdef _DEBUG
		Log::Initialize(Log::LogMode::CONSOLE);
#endif
		Input::Initialize();

		if (!impl_->window_->Init()) return false;

		if (!impl_->game_manager_->Init(impl_->window_->GetWindowHandle())) return false;

		return true;
	}

	void BaseSystem::Run(void)
	{
		impl_->MessageLoop();
	}

	void BaseSystem::Finalize(void)
	{
		Log::Finalize();

		impl_->window_->Exit();
		impl_->window_.reset();

		impl_->game_manager_->Exit();
		impl_->game_manager_.reset();

		impl_.reset();
	}
}