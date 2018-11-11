#pragma once

#include<memory>
#include<string>
#include"..\..\Framework\Entity.h"

namespace Prizm
{
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
		bool Init(const std::unique_ptr<Camera>&);
		void Run(void) override;
		void Draw(const std::unique_ptr<Camera>& camera);
		void Draw(void) override;
		void Finalize(void) override;
		bool LoadShader(const std::string&);
		bool LoadTexture(const std::string&);
	};
}