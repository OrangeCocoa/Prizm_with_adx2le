
#include"UI.h"
#include"..\..\Graphics\Graphics.h"
#include"..\..\Graphics\Geometry.h"
#include"..\..\Graphics\GeometryGenerator.h"
#include"..\..\Framework\Texture.h"
#include"..\..\Framework\Shader.h"
#include"..\..\Window\Window.h"

namespace Prizm
{
	class UI::Impl
	{
	public:
		// ToDo
		std::unique_ptr<Geometry> geometry_;
		std::shared_ptr<Shader> shader_;
		std::shared_ptr<Texture> texture_;
	};

	UI::UI(void) : impl_(std::make_unique<Impl>()) {}
	UI::~UI(void) = default;

	bool UI::Initialize()
	{
		impl_->geometry_ = std::make_unique<Geometry>(GeometryGenerator::Quad2D(256.f, 256.f, 0, 0));

		return true;
	}

	void UI::Run(void)
	{

	}

	void UI::Draw(void)
	{
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context = Graphics::GetDeviceContext();

		impl_->shader_->SetInputLayout(device_context);
		impl_->shader_->SetShader(device_context, ShaderType::VS);
		impl_->shader_->SetShader(device_context, ShaderType::PS);

		Graphics::SetRasterizerState(RasterizerStateType::CULL_NONE);
		Graphics::SetDepthStencilState(DepthStencilStateType::DEPTH_STENCIL_DISABLED);
		Graphics::SetBlendState(BlendStateType::ALIGNMENT_BLEND);

		Graphics::SetPSTexture(0, 1, impl_->texture_->GetSRV());

		device_context->PSSetSamplers(0, 1, Graphics::GetSamplerState(SamplerStateType::LINEAR_FILTER_SAMPLER).GetAddressOf());

		impl_->geometry_->Draw(device_context);
	}

	void UI::Finalize(void)
	{
		impl_->geometry_.reset();
		impl_->shader_.reset();
		impl_->texture_.reset();
	}

	void UI::LoadShader(const std::shared_ptr<Shader>& shader)
	{
		impl_->shader_ = shader;
	}

	void UI::LoadTexture(const std::shared_ptr<Texture>& texture)
	{
		impl_->texture_ = texture;
	}
	void UI::SetPosition(float x, float y)
	{
		impl_->geometry_->MovePosition2DScreenToRatio(x, y);
	}
}