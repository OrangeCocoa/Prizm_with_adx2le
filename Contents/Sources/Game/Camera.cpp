
#include<functional>

#include"Camera.h"
#include"..\Input\Input.h"
#include"..\Framework\Log.h"
#include"..\Framework\Utils.h"
#include"..\Graphics\Graphics.h"
#include"..\Graphics\ConstantBuffer.h"
#include"..\Window\Window.h"

namespace Prizm
{
	constexpr float DegreeToRadian = DirectX::XM_PI / 180.0f;

	class Camera::Impl
	{
	public:
		float yaw_, pitch_;
		DirectX::SimpleMath::Vector3 pos_, up_, look_at_, velocity_, forward_;
		DirectX::SimpleMath::Matrix rotation_;
		DirectX::SimpleMath::Vector3 translation_;
		Prism::ConstantBuffferCamera constant_buffer_;
		float overlay_alpha_;

		float drag_;
		float angular_deg_speed_;
		float move_speed_;

		float fov_;
		float aspect_;
		float near_;
		float far_;

		DirectX::SimpleMath::Vector4 fade_overlay_;	// view color

		Microsoft::WRL::ComPtr<ID3D11Buffer> cb_;

		Impl() : 
			yaw_(0), pitch_(0),
			drag_(9.f), angular_deg_speed_(0.1f), move_speed_(0.05f),
			fade_overlay_(1.0f, 1.0f, 1.0f, 1.0f)
		{
			pos_ = DirectX::SimpleMath::Vector3(0, 5, 10);
			velocity_ = DirectX::SimpleMath::Vector3(0, 0, 0);
		}

		void Move(const float dt)
		{
			auto at_dir = pos_ - look_at_;
			forward_ = look_at_ - pos_;
			forward_.Normalize();

			auto right_vector = forward_.Cross(up_);

			translation_ = DirectX::SimpleMath::Vector3(0, 0, 0);

			if (Input::IsKeyPress("W"))
			{
				pos_ += (forward_ * DirectX::SimpleMath::Vector3(move_speed_));
				look_at_ += (forward_ * DirectX::SimpleMath::Vector3(move_speed_));
			}
			if (Input::IsKeyPress("S"))
			{
				pos_ -= (forward_ * DirectX::SimpleMath::Vector3(move_speed_));
				look_at_ -= (forward_ * DirectX::SimpleMath::Vector3(move_speed_));
			}
			if (Input::IsKeyPress("A"))
			{
				pos_ += (right_vector * DirectX::SimpleMath::Vector3(move_speed_));
				look_at_ += (right_vector * DirectX::SimpleMath::Vector3(move_speed_));
			}
			if (Input::IsKeyPress("D"))
			{
				pos_ -= (right_vector * DirectX::SimpleMath::Vector3(move_speed_));
				look_at_ -= (right_vector * DirectX::SimpleMath::Vector3(move_speed_));
			}

			velocity_ += (translation_ * move_speed_ - velocity_ * drag_) * dt;
		}

		void Rotate(const float dt)
		{
			float dx = 0;
			float dy = 0;

			for (int i = 0; i < Input::max_touchcount; ++i)
			{// get touch delta (screen space)
				if (Input::IsTouchPress(i))
				{
					auto touch_delta = Input::GetTouchDelta(i);
					dx += static_cast<float>(touch_delta[0]);
					dy += static_cast<float>(touch_delta[1]);
					//Log::Info("touch_delta x: %d", dx);
					//Log::Info("touch_delta y: %d", dy);
				}
			}

			if (Input::IsMouseCaptured())
			{// get mouse delta (screen space)
				auto mouse_delta = Input::GetMouseDelta();

				dx += static_cast<float>(mouse_delta[0]);
				dy += static_cast<float>(mouse_delta[1]);
			}

			const float delta = angular_deg_speed_ * DegreeToRadian * dt;
			CameraRotate(dx, dy, delta);
		}

		void CameraRotate(float yaw, float pitch, const float dt)
		{
			yaw_ += yaw * dt;
			pitch_ += pitch * dt;

			if (pitch_ > 90.0f * DegreeToRadian) pitch_ = 90.0f * DegreeToRadian;
			if (pitch_ < -90.0f * DegreeToRadian) pitch_ = -90.0f * DegreeToRadian;
		}

		DirectX::SimpleMath::Matrix RotMatrix(void) const
		{
			return DirectX::XMMatrixRotationRollPitchYaw(pitch_, yaw_, 0);
		}

		void FadeIn()
		{

		}

		void FadeOut()
		{

		}
	};

	Camera::Camera(void) : impl_(std::make_unique<Impl>()){}

	Camera::~Camera(void) = default;

	void Camera::Initialize(void)
	{
		impl_->up_ = DirectX::SimpleMath::Vector3(0.f, 1.f, 0.f);
		impl_->look_at_ = DirectX::SimpleMath::Vector3(0.f, 0.f, 1.f);

		const auto& near_plane = 0.1f;
		const auto& far_plane = 1500.0f;
		const float aspect_ratio = window_width<float> / window_height<float>;
		const float vertical_FoV = 75 * DegreeToRadian;

		impl_->fov_ = vertical_FoV;
		impl_->aspect_ = aspect_ratio;
		impl_->near_ = near_plane;
		impl_->far_ = far_plane;

		DirectX::XMStoreFloat4x4(&impl_->constant_buffer_.view, XMMatrixLookAtLH(impl_->pos_, impl_->look_at_, impl_->up_));

		SetOthoMatrix(window_width<float>, window_height<float>, near_plane, far_plane);
		SetProjectionMatrix(vertical_FoV, aspect_ratio, near_plane, far_plane);

		impl_->yaw_ = impl_->pitch_ = 0;
		Rotate(112 * DegreeToRadian, 11 * DegreeToRadian, 1.0f);

		/*D3D11_BUFFER_DESC cb_desc;
		memset(&cb_desc, 0, sizeof(cb_desc));
		cb_desc.ByteWidth = sizeof(PrismEngine::ConstantBuffferCamera);
		cb_desc.Usage = D3D11_USAGE_DEFAULT;
		cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cb_desc.CPUAccessFlags = 0;

		if (failed(graphics_->GetDevice()->CreateBuffer(&cb_desc, nullptr, impl_->cb_.GetAddressOf())))
		{
			Log::Error("Do not create camera constant buffer.");
		}*/

		//graphics_->GetDeviceContext()->VSSetConstantBuffers(1, 1, impl_->cb_.GetAddressOf());
	}

	void Camera::Update(float dt)
	{
		impl_->Rotate(dt);
		impl_->Move(dt);

		impl_->up_ = DirectX::SimpleMath::Vector3(0, 1, 0);
		impl_->look_at_ = DirectX::SimpleMath::Vector3(0, 0, 1);

		DirectX::SimpleMath::Matrix rot = impl_->RotMatrix();
		DirectX::SimpleMath::Vector3::Transform(impl_->up_, rot, impl_->up_);
		DirectX::SimpleMath::Vector3::Transform(impl_->look_at_, rot, impl_->look_at_);

		impl_->look_at_ = impl_->pos_ + impl_->look_at_;

		DirectX::XMStoreFloat4x4(&impl_->constant_buffer_.view, XMMatrixLookAtLH(impl_->pos_, impl_->look_at_, impl_->up_));
		
		impl_->pos_ += impl_->velocity_ * dt;

		SetProjectionMatrix(impl_->fov_, impl_->aspect_, impl_->near_, impl_->far_);

		//graphics_->GetDeviceContext()->UpdateSubresource(impl_->cb_.Get(), 0, nullptr, &impl_->constant_buffer_, 0, 0);
	}

	bool Camera::FadeOut(const float & fade_frame)
	{
		const auto val = 1.0f / fade_frame;

		impl_->fade_overlay_ -= DirectX::SimpleMath::Vector4(val, val, val, 0.0f);

		return ((impl_->fade_overlay_.x <= 0.0f) & (impl_->fade_overlay_.y <= 0.0f) & (impl_->fade_overlay_.z <= 0.0f));
	}

	bool Camera::FadeIn(const float & fade_frame)
	{
		const auto val = 1.0f / fade_frame;

		impl_->fade_overlay_ += DirectX::SimpleMath::Vector4(val, val, val, 0.0f);

		return ((impl_->fade_overlay_.x >= 1.0f) & (impl_->fade_overlay_.y >= 1.0f) & (impl_->fade_overlay_.z >= 1.0f));
	}

	void Camera::SetOthoMatrix(float screen_width, float screen_height, float screen_near, float screen_far)
	{
		impl_->constant_buffer_.proj = DirectX::XMMatrixOrthographicLH(screen_width, screen_height, screen_near, screen_far);
	}

	void Camera::SetProjectionMatrix(float fov_y, float screen_aspect, float screen_near, float screen_far)
	{
		impl_->constant_buffer_.proj = DirectX::XMMatrixPerspectiveFovLH(fov_y, screen_aspect, screen_near, screen_far);
	}

	void Camera::SetProjectionMatrixHFov(float fov_x, float screen_aspect_inverse, float screen_near, float screen_far)
	{
		const float far_z = screen_far;
		const float near_z = screen_near;
		const float r = screen_aspect_inverse;

		const float width = 1.0f / tanf(fov_x*0.5f);
		const float height = width / r;
		const float range = far_z / (far_z - near_z);

		DirectX::SimpleMath::Matrix mat;

		mat._11 = width;
		mat._12 = 0.0f;
		mat._13 = 0.0f;
		mat._14 = 0.0f;

		mat._21 = 0.0f;
		mat._22 = height;
		mat._23 = 0.0f;
		mat._24 = 0.0f;

		mat._31 = 0.0f;
		mat._32 = 0.0f;
		mat._33 = range;
		mat._34 = 1.0f;

		mat._41 = 0.0f;
		mat._42 = 0.0f;
		mat._43 = -range * near_z;
		mat._44 = 0.0f;

		impl_->constant_buffer_.proj = mat;
	}

	DirectX::SimpleMath::Vector3& Camera::GetPosition(void) const
	{
		return impl_->pos_;
	}
	
	DirectX::SimpleMath::Vector3& Camera::GetAt(void) const
	{
		return impl_->look_at_;
	}

	DirectX::SimpleMath::Matrix Camera::GetViewMatrix(void) const
	{
		return impl_->constant_buffer_.view;
	}

	DirectX::SimpleMath::Matrix Camera::GetViewInverseMatrix(void) const
	{
		DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3(0.f, 1.f, 0.f);
		DirectX::SimpleMath::Vector3 look_at = DirectX::SimpleMath::Vector3(0.f, 0.f, 1.f);
		DirectX::SimpleMath::Vector3 pos = impl_->pos_;
		DirectX::SimpleMath::Matrix rot = impl_->RotMatrix();
		
		DirectX::SimpleMath::Matrix inv_view;
		impl_->constant_buffer_.view.Invert(inv_view);

		return inv_view;
	}

	DirectX::SimpleMath::Matrix Camera::GetProjectionMatrix(void) const
	{
		return impl_->constant_buffer_.proj;
	}

	void Camera::SetPosition(float x, float y, float z)
	{
		impl_->pos_ = DirectX::SimpleMath::Vector3(x, y, z);
	}

	void Camera::Rotate(float yaw, float pitch, const float dt)
	{
		impl_->CameraRotate(yaw, pitch, dt);
	}

	void Camera::Reset()
	{
		const auto& near_plane = 0.1f;
		const auto& far_plane = 1500.0f;
		const float aspect_ratio = window_width<float> / window_height<float>;
		const float vertical_FoV = 75 * DegreeToRadian;

		impl_->fov_ = vertical_FoV;
		impl_->aspect_ = aspect_ratio;
		impl_->near_ = near_plane;
		impl_->far_ = far_plane;

		SetOthoMatrix(window_width<float>, window_height<float>, near_plane, far_plane);
		SetProjectionMatrix(vertical_FoV, aspect_ratio, near_plane, far_plane);

		SetPosition(-70, 25, 50);
		impl_->yaw_ = impl_->pitch_ = 0;
		Rotate(112 * DegreeToRadian, 11 * DegreeToRadian, 1.0f);
	}
}