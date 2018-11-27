
#include"GameOverScene.h"
#include"TitleScene.h"
#include"..\SceneManager.h"
#include"..\Entity\BackGround.h"
#include"..\..\Input\Input.h"

namespace Prizm
{
	class GameOverScene::Impl
	{
	public:
		// sound id
		unsigned long bgm_;

		// texture id
		unsigned int bg_;
	};

	GameOverScene::GameOverScene() : impl_(std::make_unique<Impl>()) {}
	GameOverScene::~GameOverScene() = default;

	void GameOverScene::LoadScene(void)
	{
		impl_->bg_ = this->LoadTexture("win_vista.jpg");

		auto bg_id = this->AddBackGround<BackGround>();
		this->GetBackGround<BackGround>(bg_id)->LoadShader(this->GetShader(this->quad_shader_));
		this->GetBackGround<BackGround>(bg_id)->LoadTexture(this->GetTexture(impl_->bg_));
	}

	bool GameOverScene::Update(void)
	{
		this->RunEntities();
	
		if (Input::IsKeyTriggered("Space"))
		{
			this->GetSceneManager()->SetNextScene<TitleScene>();
			return false;
		}

		return true;
	}

	void GameOverScene::Draw(void)
	{
		this->DrawEntities();
	}

	void GameOverScene::Finalize(void)
	{
		this->FinalizeEntities();
	}
}