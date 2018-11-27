#pragma once

#include<memory>
#include"..\..\Framework\Entity.h"

namespace Prizm
{
	class Shader;
	class Texture;
	class Camera;

	class GroundField : public Entity
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> impl_;

	public:
		GroundField(void);
		~GroundField(void);

		bool Initialize(void) override;
		void Run(void) override;
		void Draw(void) override;
		void Finalize(void) override;

		void LoadShader(const std::shared_ptr<Shader>&);
		void LoadTexture(const std::shared_ptr<Texture>&);
		void SetConstantBuffer(const std::unique_ptr<Camera>&);
	};
}