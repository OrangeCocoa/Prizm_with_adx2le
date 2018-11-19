#pragma once

#include<memory>
#include<string>
#include"..\..\Framework\Entity.h"

namespace Prizm
{
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
		bool LoadShader(const std::string&);
		bool LoadTexture(const std::string&);
		void SetConstantBuffer(const std::unique_ptr<Camera>&);
	};
}