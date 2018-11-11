#pragma once

#include<memory>
#include<DirectXTK/SimpleMath.h>

namespace Prizm
{
	struct PointLight
	{
		DirectX::SimpleMath::Vector4 dir_pos;
		DirectX::SimpleMath::Vector4 color;
		DirectX::SimpleMath::Vector4 attn_power;
	};

	class Light
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		Light();
		~Light();


	};
}