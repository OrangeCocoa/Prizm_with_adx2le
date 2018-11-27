#pragma once

#include<memory>
#include"BaseScene.h"

namespace Prizm
{
	class MainGameScene : public BaseScene
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		MainGameScene(void);
		~MainGameScene(void);

		void LoadScene(void) override;
		bool Update(void) override;
		void Draw(void) override;
		void Finalize(void) override;
	};
}