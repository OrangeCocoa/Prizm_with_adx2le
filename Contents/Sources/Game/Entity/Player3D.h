#pragma once

#include<memory>
#include<DirectXTK\SimpleMath.h>

#include"..\..\Framework\Entity.h"

namespace Prizm
{
	class Shader;
	class Texture;
	class Camera;

	class Player3D : public Entity
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		Player3D(void);
		~Player3D(void);

		bool Initialize(void) override;
		void Run(void) override;
		void Draw(void) override;
		void Finalize(void) override;

		void LoadShader(const std::shared_ptr<Shader>&);
		void LoadTexture(const std::shared_ptr<Texture>&);
		void SetConstantBuffer(const std::unique_ptr<Camera>&);

		DirectX::SimpleMath::Vector3& GetPosition(void);
		void SetPosition(DirectX::SimpleMath::Vector3&);
	};
}