#pragma once

#include<memory>
#include<string>
#include"../../Framework/Entity.h"

namespace Prizm
{
	class BackGround : public Entity
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		BackGround(void);
		~BackGround();

		bool Initialize(const std::string&);
		bool Initialize() override;
		void Run(void) override;
		void Draw(void) override;
		void Finalize(void) override;
	};
}