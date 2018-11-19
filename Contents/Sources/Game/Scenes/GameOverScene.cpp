
#include"GameOverScene.h"
#include"..\Entity\BackGround.h"
#include"..\..\Input\Input.h"

namespace Prizm
{
	class GameOverScene::Impl
	{
	public:

	};

	GameOverScene::GameOverScene() : impl_(std::make_unique<Impl>()) {}
	GameOverScene::~GameOverScene() = default;

	void GameOverScene::LoadScene(void)
	{
		this->AddBackGround<BackGround>();
		this->GetBackGround<BackGround>(this->game_object_indices_[typeid(BackGround).name()].back())->LoadShader("2D.hlsl");
		this->GetBackGround<BackGround>(this->game_object_indices_[typeid(BackGround).name()].back())->LoadTexture("win_vista.jpg");
	}

	void GameOverScene::Update(void)
	{
		if (Input::IsKeyTriggered("Space"))
		{
			//this->;
		}

		this->RunEntities();
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