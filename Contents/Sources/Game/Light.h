#pragma once

#include<memory>
#include<DirectXTK/SimpleMath.h>

namespace Prizm
{
	constexpr unsigned int POINT_LIGHT_MAX = 1024;

	struct PointLight
	{
		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Vector4 color;
		DirectX::SimpleMath::Vector4 attn_power;
	};

	struct DirectionalLight
	{
		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Vector3 light_dir;
		DirectX::SimpleMath::Vector4 color;
		DirectX::SimpleMath::Vector4 attn_power;
	};

	struct SpotLight
	{
		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Vector3 light_at;
		DirectX::SimpleMath::Vector4 color;
		DirectX::SimpleMath::Vector4 attn_power;
	};
}