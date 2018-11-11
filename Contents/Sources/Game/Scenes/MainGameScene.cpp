
#include"MainGameScene.h"
#include"..\GameManager.h"
#include"..\..\Game\Camera.h"
#include"..\..\Framework\Log.h"
#include"..\..\Input\Input.h"
#include"..\..\Graphics\ImguiManager.h"
#include"..\Entity\Player3D.h"
#include"..\Entity\GroundField.h"
#include"..\Entity\BackGround.h"
//#include"..\Entity\TestUI.h"
#include"..\..\Framework\Entity.h"

namespace Prizm
{
	class MainGameScene::Impl
	{
	public:
		std::unique_ptr<Camera> main_camera_;

		std::unique_ptr<BackGround> back_ground_;

		std::unique_ptr<ImguiManager> imgui_manager_;

		Impl() : imgui_manager_(std::make_unique<ImguiManager>()) {}

		void CameraUpdate(void)
		{
			main_camera_->Update(1.f);
			imgui_manager_->SetCameraPosition(main_camera_->GetAt());
		}
	};

	MainGameScene::MainGameScene(void) : impl_(std::make_unique<Impl>()){}
	MainGameScene::~MainGameScene(void) = default;

	void MainGameScene::LoadScene(void)
	{
		this->AddGameObject<BackGround>();
		this->GetGameObject<BackGround>(this->game_object_indices_[typeid(BackGround).name()].back())->Initialize("win_7_00.jpg");

		impl_->main_camera_ = std::make_unique<Camera>();
		impl_->main_camera_->Initialize();

		impl_->back_ground_ = std::make_unique<BackGround>();

		impl_->back_ground_->Initialize();
		impl_->back_ground_->Initialize("win_7_00.jpg");

		impl_->imgui_manager_->Initialize();
	}

	void MainGameScene::Update(void)
	{
		impl_->imgui_manager_->BeginFrame();

		this->RunEntities();

		impl_->imgui_manager_->Update();
	}

	void MainGameScene::Draw(void)
	{
		impl_->CameraUpdate();
		
		// 2d back ground
		impl_->back_ground_->Draw();

		// 3d object

		// 2d ui

		//this->DrawEntities();

		impl_->imgui_manager_->EndFrame();
	}

	void MainGameScene::Finalize(void)
	{
		impl_->imgui_manager_->Exit();

		this->FinalizeEntities();
	}
}