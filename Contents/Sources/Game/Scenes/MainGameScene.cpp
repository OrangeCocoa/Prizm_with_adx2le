
#include"MainGameScene.h"
#include"GameOverScene.h"
#include"..\Camera.h"
#include"..\Light.h"
#include"..\GameManager.h"
#include"..\SceneManager.h"
#include"..\ImguiManager.h"
#include"..\Entity\Player3D.h"
#include"..\Entity\GroundField.h"
#include"..\Entity\BackGround.h"
#include"..\..\Input\Input.h"
#include"..\..\Window\Window.h"
#include"..\..\Framework\Log.h"
#include"..\..\Framework\Entity.h"
#include"..\..\Graphics\ShadowRenderer.h"
#include"..\..\Graphics\ConstantBuffer.h"
#include"..\..\Framework\Adx2\Adx2Manager.h"
#include"..\..\..\Resources\Sounds\GeneralCueSheet.h"
#include"..\..\..\Resources\Sounds\GameCueSheet.h"

namespace Prizm
{
	class MainGameScene::Impl
	{
	public:
		std::unique_ptr<SoundSource> sound_source_;
		unsigned int player_ss_;
		DirectX::SimpleMath::Vector3 velocity_;

		std::unique_ptr<Camera> main_camera_;

		PointLight point_light_;
		DirectionalLight directional_light_;

		ConstantBufferMain cb_main_;
		ShadowRenderer shadow_renderer_;

		std::unique_ptr<ImguiManager> imgui_manager_;

		// sound id
		unsigned long bgm_;
		unsigned long foot_step_;

		// texture id
		unsigned int bg_tex;
		unsigned int field_tex;
		unsigned int player_tex;
		unsigned int enemy_tex;

		// game object
		unsigned int player_obj_;

		void CameraUpdate(void)
		{
			main_camera_->Update(1.f);
			imgui_manager_->SetCameraPosition(main_camera_->GetPosition());
		}
	};

	MainGameScene::MainGameScene(void) : impl_(std::make_unique<Impl>()){}
	MainGameScene::~MainGameScene(void) = default;

	void MainGameScene::LoadScene(void)
	{
		impl_->bg_tex = this->LoadTexture("win_7_00.jpg");
		impl_->player_tex = this->LoadTexture("wall00.jpg");
		impl_->field_tex = this->LoadTexture("floor01.jpg");

		impl_->sound_source_ = std::make_unique<SoundSource>();
		impl_->imgui_manager_ = std::make_unique<ImguiManager>();
	
		impl_->directional_light_.pos = DirectX::SimpleMath::Vector3();
		impl_->directional_light_.light_dir = DirectX::SimpleMath::Vector3(1.f, -1.f, -1.f);

		// game object initialize
		auto bg_id = this->AddBackGround<BackGround>();
		this->GetBackGround<BackGround>(bg_id)->LoadShader(this->GetShader(this->quad_shader_));
		this->GetBackGround<BackGround>(bg_id)->LoadTexture(this->GetTexture(impl_->bg_tex));

		impl_->player_obj_ = this->AddGameObject3D<Player3D>();
		this->GetGameObject3D<Player3D>(impl_->player_obj_)->LoadShader(this->GetShader(this->object_shader_));
		this->GetGameObject3D<Player3D>(impl_->player_obj_)->LoadTexture(this->GetTexture(impl_->player_tex));

		auto field_id = this->AddGameObject3D<GroundField>();
		this->GetGameObject3D<GroundField>(field_id)->LoadShader(this->GetShader(this->object_shader_));
		this->GetGameObject3D<GroundField>(field_id)->LoadTexture(this->GetTexture(impl_->field_tex));

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

		// sound inport
		Adx2Manager::LoadAcb("GameCueSheet.acb", "GameCueSheet.awb", Adx2Manager::SceneAcb::GAME);
		//impl_->bgm_ = Adx2Manager::PlayCurrentCue(CRI_GAMECUESHEET_AR_MAIDIN);
		impl_->foot_step_ = Adx2Manager::PlayCurrentCue(CRI_GAMECUESHEET_AR_MAIDIN_3D);

		// sound source initialize
		impl_->player_ss_ = impl_->sound_source_->Create(Adx2Manager::GetPlayer());

		impl_->velocity_ = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
		auto def_pos = DirectX::SimpleMath::Vector3(50.0f, 0.0f, 0.0f);

		impl_->sound_source_->Update(impl_->player_ss_, def_pos, impl_->velocity_);

		this->GetGameObject3D<Player3D>(impl_->player_obj_)->SetPosition(def_pos);
	}

	bool MainGameScene::Update(void)
	{
		auto enemy_pos = this->GetGameObject3D<Player3D>(impl_->player_obj_)->GetPosition();

		float rr = pow(enemy_pos.x, 2.0f) + pow(enemy_pos.z, 2.0f);
		float acceleration = -44.f / rr;
		float r = sqrt(rr);

		impl_->velocity_.x = impl_->velocity_.x + (acceleration * enemy_pos.x / r);
		impl_->velocity_.z = impl_->velocity_.z + (acceleration * enemy_pos.z / r);

		enemy_pos.x = enemy_pos.x + impl_->velocity_.x;
		enemy_pos.z = enemy_pos.z + impl_->velocity_.z;

		this->GetGameObject3D<Player3D>(impl_->player_obj_)->SetPosition(enemy_pos);

		auto player_front = impl_->main_camera_->GetAt();
		player_front.y = 0.f;
		player_front = enemy_pos - impl_->main_camera_->GetPosition();
		auto player_up = DirectX::SimpleMath::Vector3::Up;

		impl_->sound_source_->Update(impl_->player_ss_, enemy_pos, impl_->velocity_);
		Adx2Manager::PositioningSoundListenerUpdate(impl_->main_camera_->GetPosition(), player_front, player_up);

		if (Input::IsKeyTriggered("Space"))
		{
			Adx2Manager::RestorePitchAll();
			Adx2Manager::StopCue(impl_->bgm_);
			this->GetSceneManager()->SetNextScene<GameOverScene>();
			return false;
		}

		if (Input::IsKeyTriggered("P"))
		{
			static int pause_status = 0;
			Adx2Manager::PauseCue(impl_->bgm_, pause_status ^= 1);
		}

		impl_->imgui_manager_->BeginFrame();

		this->RunEntities();

		impl_->imgui_manager_->Update();

		return true;
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
		Adx2Manager::ReleaseAcb(Adx2Manager::SceneAcb::GAME);

		impl_->imgui_manager_->Finalize();

		this->FinalizeEntities();
	}
}