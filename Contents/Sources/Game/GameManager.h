#pragma once

#include<windows.h>
#include<memory>

namespace Prizm
{
	class GameManager
	{
		friend class BaseScene;

	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		GameManager(void);
		~GameManager(void);

		bool Initialize(HWND);
		bool Run(void);
		void Finalize(void);
	};
}