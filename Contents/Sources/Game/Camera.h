#pragma once

#include<memory>
#include<DirectXTK/SimpleMath.h>
#include<DirectXMathMatrix.inl>
#include<DirectXMathVector.inl>

namespace Prizm
{
	class Camera
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		Camera(void);
		~Camera(void);

		void Initialize(void);
		void Update(float dt);

		bool FadeOut(const float& fade_frame);
		bool FadeIn(const float& fade_frame);

		void SetOthoMatrix(float screen_width, float screen_height, float screen_near, float screen_far);
		void SetProjectionMatrix(float fov_y, float screen_aspect, float screen_near, float screen_far);
		void SetProjectionMatrixHFov(float fov_x, float screen_aspect_inverse, float screen_near, float screen_far);

		DirectX::SimpleMath::Vector3& GetPosition(void) const;
		DirectX::SimpleMath::Vector3& GetAt(void) const;
		DirectX::SimpleMath::Matrix& GetViewMatrix(void) const;
		DirectX::SimpleMath::Matrix GetViewInverseMatrix(void) const;
		DirectX::SimpleMath::Matrix& GetProjectionMatrix(void) const;
		DirectX::SimpleMath::Matrix& GetViewProjectionMatrix(void) const;

		DirectX::SimpleMath::Vector2 GetNearFarPlane(void) const;

		void SetPosition(float x, float y, float z);
		void Rotate(float yaw, float pitch, const float dt);

		void Reset();
	};
}