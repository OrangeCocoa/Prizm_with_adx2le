#pragma once

#include<DirectXTK/SimpleMath.h>

namespace Prizm
{
	struct PointLight
	{
		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Vector4 color;
		DirectX::SimpleMath::Vector4 attn;
	};

	class Light
	{
	private:

	public:

	};
}