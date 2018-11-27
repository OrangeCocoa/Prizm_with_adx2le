
#include"Player3D.h"

#include"..\Camera.h"
#include"..\..\Graphics\ConstantBuffer.h"
#include"..\..\Graphics\Graphics.h"
#include"..\..\Graphics\Geometry.h"
#include"..\..\Graphics\GeometryGenerator.h"
#include"..\..\Framework\Shader.h"
#include"..\..\Framework\Texture.h"
#include"..\..\Framework\Buffer.h"
#include"..\..\Framework\Log.h"
#include"..\..\Input\Input.h"

namespace Prizm
{
	class Player3D::Impl
	{
	public:
		std::unique_ptr<Geometry> geometry_;
		std::shared_ptr<Shader> shader_;
		std::shared_ptr<Texture> texture_;

		DirectX::SimpleMath::Vector3 pos_;

		Buffer cb_data_;
		CostantBufferMatrix3DSimple cb_;
		ConstantBufferObject cb_object_;

		Impl() : shader_(std::make_unique<Shader>()){}
	};

	Player3D::Player3D() : impl_(std::make_unique<Impl>())
	{
		impl_->texture_ = std::make_unique<Texture>();
	}

	Player3D::~Player3D(void) = default;

	bool Player3D::Initialize(void)
	{
		impl_->geometry_ = std::make_unique<Geometry>(GeometryGenerator::Cube());

		impl_->pos_ = DirectX::SimpleMath::Vector3::Zero;

		impl_->cb_.world = DirectX::SimpleMath::Matrix::Identity;
		impl_->cb_.view = DirectX::SimpleMath::Matrix::Identity;
		impl_->cb_.proj = DirectX::SimpleMath::Matrix::Identity;

		impl_->cb_data_ = impl_->shader_->CreateConstantBuffer(Graphics::GetDevice(), impl_->cb_);

		return true;
	}

	void Player3D::Run(void)
	{
		DirectX::SimpleMath::Matrix trans, rot_X, rot_Y;

		trans = DirectX::SimpleMath::Matrix::CreateTranslation(impl_->pos_);

		static float Y = 0.0f;
		Y += 0.02f;

		if (Input::IsTouchPress(Input::max_touchcount - 1))
		{
			Y += 10.0f;
		}

		DirectX::XMStoreFloat4x4(&rot_X, DirectX::XMMatrixRotationX(Y * DirectX::XM_PI / 180.0f));
		DirectX::XMStoreFloat4x4(&rot_Y, DirectX::XMMatrixRotationY(Y * DirectX::XM_PI / 180.0f));

		impl_->cb_.world = rot_Y * rot_X * trans;
	}

	void Player3D::Draw(void)
	{
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context = Graphics::GetDeviceContext();

		impl_->shader_->SetInputLayout(device_context);
		impl_->shader_->SetShader(device_context, ShaderType::VS);
		impl_->shader_->SetShader(device_context, ShaderType::PS);

		Graphics::SetRasterizerState(RasterizerStateType::CULL_BACK);
		Graphics::SetDepthStencilState(DepthStencilStateType::DEPTH_STENCIL_WRITE);
		Graphics::SetBlendState(BlendStateType::ALIGNMENT_BLEND);

		Graphics::SetPSTexture(0, 1, impl_->texture_->GetSRV());

		device_context->PSSetSamplers(0, 1, Graphics::GetSamplerState(SamplerStateType::LINEAR_FILTER_SAMPLER).GetAddressOf());

		impl_->shader_->UpdateConstantBuffer(Graphics::GetDeviceContext(), impl_->cb_data_, impl_->cb_, ShaderType::VS, 0, 1);
		impl_->geometry_->Draw(device_context);
	}

	void Player3D::Finalize(void)
	{
		impl_->geometry_.reset();
		impl_->shader_.reset();
		impl_->texture_.reset();
	}

	void Player3D::LoadShader(const std::shared_ptr<Shader>& shader)
	{
		impl_->shader_ = shader;
	}

	void Player3D::LoadTexture(const std::shared_ptr<Texture>& texture)
	{
		impl_->texture_ = texture;
	}

	void Player3D::SetConstantBuffer(const std::unique_ptr<Camera>& camera)
	{
		impl_->cb_.view = camera->GetViewMatrix();
		impl_->cb_.proj = camera->GetProjectionMatrix();
	}

	DirectX::SimpleMath::Vector3& Player3D::GetPosition(void)
	{
		return impl_->pos_;
	}
	void Player3D::SetPosition(DirectX::SimpleMath::Vector3& pos)
	{
		impl_->pos_ = pos;
	}
}