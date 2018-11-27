
#include"GroundField.h"

#include"..\Camera.h"
#include"..\..\Graphics\Graphics.h"
#include"..\..\Graphics\Geometry.h"
#include"..\..\Graphics\GeometryGenerator.h"
#include"..\..\Framework\Shader.h"
#include"..\..\Framework\Texture.h"
#include"..\..\Framework\Buffer.h"
#include"..\..\Framework\Log.h"
#include"..\..\Graphics\ConstantBuffer.h"
#include"..\..\Input\Input.h"

namespace Prizm
{
	struct GroundField::Impl
	{
		std::unique_ptr<Geometry> geometry_;
		std::shared_ptr<Shader> shader_;
		std::shared_ptr<Texture> texture_;

		Buffer cb_data_;
		CostantBufferMatrix3DSimple cb_;

		Impl(void)
			: shader_(std::make_unique<Shader>())
			, texture_(std::make_unique<Texture>()){}
	};

	GroundField::GroundField(void) : impl_(std::make_unique<Impl>()){}
	GroundField::~GroundField(void) = default;

	bool GroundField::Initialize(void)
	{
		impl_->geometry_ = std::make_unique<Geometry>(GeometryGenerator::QuadFieldList(100, 100));

		impl_->cb_.world = DirectX::SimpleMath::Matrix::Identity;
		impl_->cb_.view = DirectX::SimpleMath::Matrix::Identity;
		impl_->cb_.proj = DirectX::SimpleMath::Matrix::Identity;

		impl_->cb_data_ = impl_->shader_->CreateConstantBuffer(Graphics::GetDevice(), impl_->cb_);

		return true;
	}

	void GroundField::Run(void)
	{
	}

	void GroundField::Draw(void)
	{
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> dc = Graphics::GetDeviceContext();

		impl_->shader_->SetInputLayout(dc);
		impl_->shader_->SetShader(dc, ShaderType::VS);
		impl_->shader_->SetShader(dc, ShaderType::PS);

		Graphics::SetRasterizerState(RasterizerStateType::CULL_NONE);
		Graphics::SetDepthStencilState(DepthStencilStateType::DEPTH_STENCIL_WRITE);
		Graphics::SetBlendState(BlendStateType::ALIGNMENT_BLEND);

		Graphics::SetPSTexture(0, 1, impl_->texture_->GetSRV());

		dc->PSSetSamplers(0, 1, Graphics::GetSamplerState(SamplerStateType::LINEAR_FILTER_SAMPLER).GetAddressOf());

		impl_->shader_->UpdateConstantBuffer(dc, impl_->cb_data_, impl_->cb_, ShaderType::VS, 0, 1);
		impl_->geometry_->Draw(dc);
	}

	void GroundField::Finalize(void)
	{
		impl_->geometry_.reset();
		impl_->shader_.reset();
		impl_->texture_.reset();
	}

	void GroundField::LoadShader(const std::shared_ptr<Shader>& shader)
	{
		impl_->shader_ = shader;
	}

	void GroundField::LoadTexture(const std::shared_ptr<Texture>& texture)
	{
		impl_->texture_ = texture;
	}

	void GroundField::SetConstantBuffer(const std::unique_ptr<Camera>& camera)
	{
		DirectX::SimpleMath::Matrix matTrans;

		DirectX::XMStoreFloat4x4(&matTrans, DirectX::XMMatrixTranslationFromVector(DirectX::SimpleMath::Vector3(0.0f, -4.0f, 0.0f)));

		impl_->cb_.world = matTrans;
		impl_->cb_.view = camera->GetViewMatrix();
		impl_->cb_.proj = camera->GetProjectionMatrix();
	}
}