
#include"Player3D.h"

#include"..\Camera.h"
#include"..\..\Graphics\ConstantBuffer.h"
#include"..\..\Graphics\Graphics.h"
#include"..\..\Graphics\Geometry.h"
#include"..\..\Graphics\GeometryGenerator.h"
#include"..\..\Graphics\Shader.h"
#include"..\..\Graphics\Texture.h"
#include"..\..\Input\Input.h"
#include"..\..\Framework\Log.h"

namespace Prizm
{
	class Player3D::Impl
	{
	public:
		std::unique_ptr<Geometry> geometry_;
		std::unique_ptr<Shader> shader_;
		std::unique_ptr<Texture> texture_;

		unsigned int object_shader_;
		unsigned int player_texture_;

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

		impl_->cb_.world = DirectX::SimpleMath::Matrix::Identity;
		impl_->cb_.view = DirectX::SimpleMath::Matrix::Identity;
		impl_->cb_.proj = DirectX::SimpleMath::Matrix::Identity;

		impl_->shader_->CreateConstantBuffer(Graphics::GetDevice(), impl_->cb_);

		Log::Info("Player3D initialize succeeded.");

		return true;
	}

	void Player3D::Run(void)
	{

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

		impl_->texture_->SetPSTexture(0, 1);

		device_context->PSSetSamplers(0, 1, Graphics::GetSamplerState(SamplerStateType::LINEAR_FILTER_SAMPLER).GetAddressOf());

		impl_->shader_->UpdateConstantBuffer(Graphics::GetDeviceContext(), impl_->cb_, ShaderType::VS, 0, 1);
		impl_->geometry_->Draw(device_context);
	}

	void Player3D::Finalize(void)
	{
		impl_->geometry_.reset();
		impl_->shader_.reset();
		impl_->texture_.reset();
	}

	bool Player3D::LoadShader(const std::string& shader_name)
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

	bool Player3D::LoadTexture(const std::string& tex_name)
	{
		impl_->texture_->LoadTexture(tex_name);

		return true;
	}

	void Player3D::SetConstantBuffer(const std::unique_ptr<Camera>& camera)
	{
		DirectX::SimpleMath::Matrix matRotX, matRotY;

		static float Y = 0.0f;
		Y += 0.02f;

		if (Input::IsTouchPress(Input::max_touchcount - 1))
		{
			Y += 10.0f;
		}

		DirectX::XMStoreFloat4x4(&matRotX, DirectX::XMMatrixRotationX(Y * DirectX::XM_PI / 180.0f));
		DirectX::XMStoreFloat4x4(&matRotY, DirectX::XMMatrixRotationY(Y * DirectX::XM_PI / 180.0f));

		impl_->cb_.world = matRotY * matRotX;
		impl_->cb_.view = camera->GetViewMatrix();
		impl_->cb_.proj = camera->GetProjectionMatrix();
	}
}