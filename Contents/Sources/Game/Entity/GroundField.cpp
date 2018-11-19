
#include"GroundField.h"

#include"..\..\Graphics\Graphics.h"
#include"..\..\Graphics\Geometry.h"
#include"..\..\Graphics\GeometryGenerator.h"
#include"..\..\Graphics\Shader.h"
#include"..\..\Graphics\Texture.h"
#include"..\..\Graphics\ConstantBuffer.h"
#include"..\Camera.h"
#include"..\..\Input\Input.h"
#include"..\..\Framework\Log.h"

namespace Prizm
{
	struct GroundField::Impl
	{
		std::unique_ptr<Geometry> geometry_;
		std::unique_ptr<Shader> shader_;
		std::unique_ptr<Texture> texture_;

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

		impl_->shader_->CreateConstantBuffer(Graphics::GetDevice(), impl_->cb_);

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

		impl_->texture_->SetPSTexture(0, 1);

		dc->PSSetSamplers(0, 1, Graphics::GetSamplerState(SamplerStateType::LINEAR_FILTER_SAMPLER).GetAddressOf());

		impl_->shader_->UpdateConstantBuffer(dc, impl_->cb_, ShaderType::VS, 0, 1);
		impl_->geometry_->Draw(dc);
	}

	void GroundField::Finalize(void)
	{
		impl_->geometry_.reset();
		impl_->shader_.reset();
		impl_->texture_.reset();
	}

	bool GroundField::LoadShader(const std::string& shader_name)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> def_element =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, 52, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (!impl_->shader_->CompileAndCreateFromFile(Graphics::GetDevice(), shader_name, ShaderType::VS, def_element)) return false;
		if (!impl_->shader_->CompileAndCreateFromFile(Graphics::GetDevice(), shader_name, ShaderType::PS, def_element)) return false;

		return true;
	}
	bool GroundField::LoadTexture(const std::string& tex_path)
	{
		impl_->texture_->LoadTexture(tex_path);

		return true;
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