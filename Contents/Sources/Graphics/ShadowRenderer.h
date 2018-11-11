#pragma once

#include<memory>

namespace Prizm
{
	class ShadowRenderer
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		ShadowRenderer();
		~ShadowRenderer();

		bool Initialize(void);
		void Render(void);
	};
}