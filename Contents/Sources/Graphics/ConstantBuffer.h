#pragma once

#include<DirectXTK/SimpleMath.h>

namespace Prism
{
	struct CostantBuffer2D
	{// UI
		DirectX::SimpleMath::Vector2 screen_pos;
		DirectX::SimpleMath::Vector2 size;			// width and height
		DirectX::SimpleMath::Vector2 uv;
		float alpha;
	};
	
	struct CostantBuffer3D
	{
		DirectX::SimpleMath::Vector4 pos;
		DirectX::SimpleMath::Vector3 normal;
		DirectX::SimpleMath::Vector2 uv;
		DirectX::SimpleMath::Vector4 color;
	};

	struct CostantBufferMatrix3D
	{
		DirectX::SimpleMath::Matrix world;
		DirectX::SimpleMath::Vector4 diffuse;
		DirectX::SimpleMath::Vector4 emissive;
	};

	struct CostantBufferMatrix3DSimple
	{
		DirectX::SimpleMath::Matrix world;
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix proj;
	};
	
	struct ConstantBuffferCamera
	{
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix proj;
	};
}