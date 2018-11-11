#pragma once

#include<memory>
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

		bool Init(const std::unique_ptr<Camera>&);
		void Run(void);
		void Draw(const std::unique_ptr<Camera>&);
		bool LoadShader(void);
		//void SetShader(std::unique_ptr<Shader>&);
	};
}