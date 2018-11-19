
#include"MainGameScene.h"
#include"..\GameManager.h"
#include"..\..\Input\Input.h"
#include"..\..\Window\Window.h"
#include"..\..\Framework\Log.h"
#include"..\..\Game\Camera.h"
#include"..\Light.h"
#include"..\..\Graphics\ImguiManager.h"
#include"..\Entity\Player3D.h"
#include"..\Entity\GroundField.h"
#include"..\Entity\BackGround.h"
#include"..\..\Framework\Entity.h"
#include"..\..\Graphics\ShadowRenderer.h"
#include"..\..\Graphics\ConstantBuffer.h"

namespace Prizm
{
	class MainGameScene::Impl
	{
	public:
		std::unique_ptr<Camera> main_camera_;

		PointLight point_light_;
		DirectionalLight directional_light_;

		ConstantBufferMain cb_main_;
		ShadowRenderer shadow_renderer_;

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
		impl_->directional_light_.pos = DirectX::SimpleMath::Vector3();
		impl_->directional_light_.light_dir = DirectX::SimpleMath::Vector3(1.f, -1.f, -1.f);


		this->AddBackGround<BackGround>();
		this->GetBackGround<BackGround>(this->game_object_indices_[typeid(BackGround).name()].back())->LoadShader("2D.hlsl");
		this->GetBackGround<BackGround>(this->game_object_indices_[typeid(BackGround).name()].back())->LoadTexture("win_7_00.jpg");

		this->AddGameObject3D<Player3D>();
		this->GetGameObject3D<Player3D>(this->game_object_indices_[typeid(Player3D).name()].back())->LoadShader("Texture.hlsl");
		this->GetGameObject3D<Player3D>(this->game_object_indices_[typeid(Player3D).name()].back())->LoadTexture("wall00.jpg");

		this->AddGameObject3D<GroundField>();
		this->GetGameObject3D<GroundField>(this->game_object_indices_[typeid(GroundField).name()].back())->LoadShader("Texture.hlsl");
		this->GetGameObject3D<GroundField>(this->game_object_indices_[typeid(GroundField).name()].back())->LoadTexture("floor01.jpg");

		impl_->main_camera_ = std::make_unique<Camera>();
		impl_->main_camera_->Initialize();

		impl_->imgui_manager_->Initialize();

		// constant param
		impl_->cb_main_.dir_light_pos = impl_->directional_light_.pos;
		
		/*impl_->cb_main_.light_view = DirectX::SimpleMath::Matrix::CreateLookAt(
			impl_->directional_light_.pos, DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0, 1, 0));
*/
		impl_->cb_main_.light_proj = 
			DirectX::SimpleMath::Matrix::CreateOrthographic(
				window_width<float>, window_height<float>
				, impl_->main_camera_->GetNearFarPlane().x, impl_->main_camera_->GetNearFarPlane().y);

		// update param
		impl_->cb_main_.eye_pos = impl_->main_camera_->GetPosition();
		impl_->cb_main_.view = impl_->main_camera_->GetViewMatrix();
		impl_->cb_main_.proj = impl_->main_camera_->GetProjectionMatrix();
		impl_->cb_main_.timer = 0;

		impl_->shadow_renderer_.Initialize();
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

		this->GetGameObject3D<Player3D>(this->game_object_indices_[typeid(Player3D).name()].back())->SetConstantBuffer(impl_->main_camera_);
		this->GetGameObject3D<GroundField>(this->game_object_indices_[typeid(GroundField).name()].back())->SetConstantBuffer(impl_->main_camera_);

		this->DrawEntities();

		impl_->imgui_manager_->EndFrame();
	}

	void MainGameScene::Finalize(void)
	{
		impl_->imgui_manager_->Finalize();

		this->FinalizeEntities();
	}
}