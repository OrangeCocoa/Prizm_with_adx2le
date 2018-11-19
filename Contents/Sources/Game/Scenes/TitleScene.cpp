
#include"TitleScene.h"
#include"..\Entity\BackGround.h"
#include"..\..\Input\Input.h"

namespace Prizm
{
	class TitleScene::Impl
	{
	public:

	};

	TitleScene::TitleScene() : impl_(std::make_unique<Impl>()){}
	TitleScene::~TitleScene() = default;

	void TitleScene::LoadScene(void)
	{
		this->AddBackGround<BackGround>();
		this->GetBackGround<BackGround>(this->game_object_indices_[typeid(BackGround).name()].back())->LoadShader("2D.hlsl");
		this->GetBackGround<BackGround>(this->game_object_indices_[typeid(BackGround).name()].back())->LoadTexture("win_7_01.jpg");
	}

	void TitleScene::Update(void)
	{
		if (Input::IsKeyTriggered("Space"))
		{
			//this->;
		}

		this->RunEntities();
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