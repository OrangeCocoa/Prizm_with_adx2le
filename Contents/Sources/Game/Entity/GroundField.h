#pragma once

#include<memory>
#include"..\..\Framework\Entity.h"

namespace Prizm
{
	class Camera;

	class GroundField// : public Entity
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> impl_;

	public:
		GroundField(void);
		~GroundField(void);

		bool Initialize(const std::unique_ptr<Camera>&);
		void Run(void);
		void Draw(const std::unique_ptr<Camera>&);
		bool LoadShader(void);
	};
}