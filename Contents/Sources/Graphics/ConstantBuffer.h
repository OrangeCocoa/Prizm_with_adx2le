#pragma once

#include<DirectXTK/SimpleMath.h>

namespace Prizm
{
	// UI ?
	struct CostantBuffer2D
	{
		DirectX::SimpleMath::Vector2 screen_pos;
		DirectX::SimpleMath::Vector2 size;			// width and height
		DirectX::SimpleMath::Vector2 uv;
		float alpha;
	};
	
	// 3D object matrix
	struct CostantBufferMatrix3D
	{
		DirectX::SimpleMath::Matrix WVP;
		DirectX::SimpleMath::Vector4 diffuse;
		DirectX::SimpleMath::Vector4 emissive;
	};

	// 3D object matrix not lighting
	struct CostantBufferMatrix3DSimple
	{
		DirectX::SimpleMath::Matrix world;
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix proj;
	};
	
	// camera 
	struct ConstantBuffferCamera
	{
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix proj;
		float						overlay_alpha_;
	};

	// scene render
	struct ConstantBuffferScene
	{
		DirectX::SimpleMath::Matrix		view;
		DirectX::SimpleMath::Matrix		proj;
		DirectX::SimpleMath::Vector4	screen_param;
		DirectX::SimpleMath::Vector4	frustum_corner;
		DirectX::SimpleMath::Matrix		last_view;
		DirectX::SimpleMath::Matrix		last_proj;
		float							exposureTime;
		int								pad[3];
	};

	// light
	struct ConstantBuffferPointLight
	{
		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Vector4 color;
		DirectX::SimpleMath::Vector4 atten;
	};

	// shadow map
	struct ConstantBufferShadow1
	{// shadow map view project
		DirectX::SimpleMath::Matrix light_view;
		DirectX::SimpleMath::Matrix light_proj;
	};

	struct ConstantBufferShadow2
	{// render mesh shadow's
		DirectX::SimpleMath::Matrix world;
	};

	// centrailization constant buffer
	struct ConstantBufferMain
	{
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix proj;
		DirectX::SimpleMath::Matrix light_view;
		DirectX::SimpleMath::Matrix light_proj;
		DirectX::SimpleMath::Vector3 eye_pos;
		DirectX::SimpleMath::Vector3 dir_light_pos;
		__declspec(align(16)) unsigned int timer;
	};

	struct ConstantBufferObject
	{
		DirectX::SimpleMath::Matrix world;
		DirectX::SimpleMath::Vector4 color;
	};
}