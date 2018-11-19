#pragma once

#include<memory>
#include"BaseScene.h"

namespace Prizm
{
	class GameOverScene : public BaseScene
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;
	public:
		GameOverScene();
		~GameOverScene();

		void LoadScene(void) override;
		void Update(void) override;
		void Draw(void) override;
		void Finalize(void) override;
	};
}