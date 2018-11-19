
#include"BackGround.h"
#include"..\..\Graphics\Geometry.h"
#include"..\..\Graphics\GeometryGenerator.h"
#include"..\..\Graphics\Texture.h"
#include"..\..\Graphics\Shader.h"
#include"..\..\Window\Window.h"

namespace Prizm
{
	class BackGround::Impl
	{
	public:
		std::unique_ptr<Geometry> geometry_;
		std::unique_ptr<Shader> shader_;
		std::unique_ptr<Texture> texture_;

		Impl(void)
			: shader_(std::make_unique<Shader>())
			, texture_(std::make_unique<Texture>())
		{}
	};

	BackGround::BackGround(void) : impl_(std::make_unique<Impl>()) {}
	BackGround::~BackGround() = default;

	bool BackGround::Initialize(void)
	{
		impl_->geometry_ = std::make_unique<Geometry>(GeometryGenerator::Quad2D(window_width<float>, window_height<float>, 0, 0));

		return true;
	}

	void BackGround::Run(void)
	{

	}

	void BackGround::Draw(void)
	{
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context = Graphics::GetDeviceContext();

		impl_->shader_->SetInputLayout(device_context);
		impl_->shader_->SetShader(device_context, ShaderType::VS);
		impl_->shader_->SetShader(device_context, ShaderType::PS);

		Graphics::SetRasterizerState(RasterizerStateType::CULL_NONE);
		Graphics::SetDepthStencilState(DepthStencilStateType::DEPTH_STENCIL_DISABLED);
		Graphics::SetBlendState(BlendStateType::ALIGNMENT_BLEND);

		impl_->texture_->SetPSTexture(0, 1);

		device_context->PSSetSamplers(0, 1, Graphics::GetSamplerState(SamplerStateType::LINEAR_FILTER_SAMPLER).GetAddressOf());

		impl_->geometry_->Draw(device_context);
	}
	void BackGround::Finalize(void)
	{
		impl_->geometry_.reset();
		impl_->shader_.reset();
		impl_->texture_.reset();
	}

	bool BackGround::LoadShader(const std::string& shader_name)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> def_element =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (!impl_->shader_->CompileAndCreateFromFile(Graphics::GetDevice(), shader_name, ShaderType::VS, def_element)) return false;
		if (!impl_->shader_->CompileAndCreateFromFile(Graphics::GetDevice(), shader_name, ShaderType::PS, def_element)) return false;
		return true;
	}

	bool BackGround::LoadTexture(const std::string& tex_name)
	{
		impl_->texture_->LoadTexture(tex_name);

		return true;
	}
}