#pragma once

#include<memory>
#include"../../Framework/Entity.h"

namespace Prizm
{
	class Shader;
	class Texture;

	class UI : public Entity
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		UI(void);
		~UI(void);

		bool Initialize() override;
		void Run(void) override;
		void Draw(void) override;
		void Finalize(void) override;

		void LoadShader(const std::shared_ptr<Shader>&);
		void LoadTexture(const std::shared_ptr<Texture>&);

		void SetPosition(float x, float y);
	};
}