
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

		return true;
	}

	bool Player3D::Init(const std::unique_ptr<Camera>& camera)
	{
		CostantBufferMatrix3DSimple cb;

		cb.world = DirectX::SimpleMath::Matrix::Identity;
		cb.view = camera->GetViewMatrix();
		cb.proj = camera->GetProjectionMatrix();

		impl_->shader_->CreateConstantBuffer(Graphics::GetDevice(), cb);

		LoadShader("Texture.hlsl");

		Log::Info("Player3D initialize succeeded.");
		return true;
	}

	void Player3D::Run(void)
	{

	}

	void Player3D::Draw(const std::unique_ptr<Camera>& camera)
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

		CostantBufferMatrix3DSimple cb;
		cb.world = matRotY * matRotX;
		cb.view = camera->GetViewMatrix();
		cb.proj = camera->GetProjectionMatrix();

		Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context = Graphics::GetDeviceContext();

		impl_->shader_->SetInputLayout(device_context);
		impl_->shader_->SetShader(device_context, ShaderType::VS);
		impl_->shader_->SetShader(device_context, ShaderType::PS);

		Graphics::SetRasterizerState(RasterizerStateType::CULL_BACK);
		Graphics::SetDepthStencilState(DepthStencilStateType::DEPTH_WRITE);
		Graphics::SetBlendState(BlendStateType::ALIGNMENT_BLEND);

		impl_->texture_->SetPSTexture(0, 1);

		device_context->PSSetSamplers(0, 1, Graphics::GetSamplerState(SamplerStateType::LINEAR_FILTER_SAMPLER).GetAddressOf());

		impl_->shader_->UpdateConstantBuffer(Graphics::GetDeviceContext(), cb, ShaderType::VS, 0, 1);
		impl_->geometry_->Draw(device_context);
	}

	void Player3D::Draw(void)
	{
	}

	void Player3D::Finalize(void)
	{
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
		impl_->texture_->LoadTexture(tex_name); //"wall00.jpg"

		return true;
	}
}