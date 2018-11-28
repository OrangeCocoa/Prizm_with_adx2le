
#include"TitleScene.h"
#include"MainGameScene.h"
#include"..\SceneManager.h"
#include"..\Entity\BackGround.h"
#include"..\Entity\UI.h"
#include"..\..\Input\Input.h"
#include"..\..\Framework\Adx2\Adx2Manager.h"
#include"..\..\..\Resources\Sounds\GeneralCueSheet.h"

namespace Prizm
{
	class TitleScene::Impl
	{
	public:
		// sound id
		unsigned long bmg_id_;

		// texture id
		unsigned int bg_;
		unsigned int cri_;
	};

	TitleScene::TitleScene() : impl_(std::make_unique<Impl>()){}
	TitleScene::~TitleScene() = default;

	void TitleScene::LoadScene(void)
	{
		impl_->bg_ = this->LoadTexture("win_7_01.jpg");
		impl_->cri_ = this->LoadTexture("criware.png");

		auto bg_id = this->AddBackGround<BackGround>();
		this->GetBackGround<BackGround>(bg_id)->LoadShader(this->GetShader(this->quad_shader_));
		this->GetBackGround<BackGround>(bg_id)->LoadTexture(this->GetTexture(impl_->bg_));

		auto ui_id = this->AddGameObject2D<UI>();
		this->GetGameObject2D<UI>(ui_id)->LoadShader(this->GetShader(this->quad_shader_));
		this->GetGameObject2D<UI>(ui_id)->LoadTexture(this->GetTexture(impl_->cri_));
		this->GetGameObject2D<UI>(ui_id)->SetPosition(-700.f, -400.f);

		impl_->bmg_id_ = Adx2Manager::PlayGeneralCue(CRI_GENERALCUESHEET_BOY_MEETS_GIRL);
	}

	bool TitleScene::Update(void)
	{
		this->RunEntities();

		if (Input::IsKeyTriggered("Space"))
		{
			Adx2Manager::RestorePitchAll();
			Adx2Manager::StopCue(impl_->bmg_id_);
			this->GetSceneManager()->SetNextScene<MainGameScene>();
			return false;
		}

		return true;
	}

	void TitleScene::Draw(void)
	{
		this->DrawEntities();
	}

	void TitleScene::Finalize(void)
	{
		this->FinalizeEntities();
	}
}