
#include"GroundField.h"

#include"..\..\Graphics\Graphics.h"
#include"..\..\Graphics\Geometry.h"
#include"..\..\Graphics\GeometryGenerator.h"
#include"..\..\Graphics\Shader.h"
#include"..\..\Graphics\Texture.h"
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

		Impl(void)
			: shader_(std::make_unique<Shader>())
			, texture_(std::make_unique<Texture>()){}
	};


	GroundField::GroundField(void) : impl_(std::make_unique<Impl>()){}
	GroundField::~GroundField(void) = default;

	bool GroundField::Init(const std::unique_ptr<Camera>& camera)
	{
		impl_->geometry_ = std::make_unique<Geometry>(GeometryGenerator::QuadFieldList(100, 100));

		Prism::CostantBufferMatrix3DSimple cb;

		cb.world = DirectX::SimpleMath::Matrix::Identity;
		cb.view = camera->GetViewMatrix();
		cb.proj = camera->GetProjectionMatrix();

		impl_->texture_->LoadTexture("floor01.jpg");

		LoadShader();

		impl_->shader_->CreateConstantBuffer<Prism::CostantBufferMatrix3DSimple>(Graphics::GetDevice(), cb);

		return true;
	}

	void GroundField::Run(void)
	{
	}

	void GroundField::Draw(const std::unique_ptr<Camera>& camera)
	{
		/*Prism::CostantBufferMatrix3DSimple cb;
		DirectX::SimpleMath::Matrix matTrans;

		DirectX::XMStoreFloat4x4(&matTrans, DirectX::XMMatrixTranslationFromVector(DirectX::SimpleMath::Vector3(0.0f, -4.0f, 0.0f)));

		cb.world = matTrans;
		cb.view = camera->GetViewMatrix();
		cb.proj = camera->GetProjectionMatrix();

		Microsoft::WRL::ComPtr<ID3D11DeviceContext> dc = impl_->graphics_->GetDeviceContext();

		impl_->shader_->SetInputLayout(dc);
		impl_->shader_->SetVertexShader(dc);
		impl_->shader_->SetPixelShader(dc);

		impl_->graphics_->SetRasterizerState(RasterizerStateType::CULL_NONE);
		impl_->graphics_->SetDepthStencilState(DepthStencilStateType::DEPTH_WRITE);
		impl_->graphics_->SetBlendState(BlendStateType::ALIGNMENT_BLEND);

		impl_->texture_->SetPSTexture(0, 1);

		dc->PSSetSamplers(0, 1, impl_->graphics_->GetSamplerState(SamplerStateType::LINEAR_FILTER_SAMPLER).GetAddressOf());

		impl_->geometry_->UpdateConstantBuffer(dc, cb, 0, 1);
		impl_->geometry_->Draw(dc);*/
	}

	bool GroundField::LoadShader(void)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> def_element =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, 52, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (!impl_->shader_->CompileAndCreateFromFile(Graphics::GetDevice(), "Texture.hlsl", ShaderType::VS, def_element)) return false;
		if (!impl_->shader_->CompileAndCreateFromFile(Graphics::GetDevice(), "Texture.hlsl", ShaderType::PS, def_element)) return false;

		return true;
	}
}