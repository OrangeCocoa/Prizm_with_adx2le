
#include"ShadowRenderer.h"
#include"Graphics.h"
#include"Shader.h"
#include"ConstantBuffer.h"

namespace Prizm
{
	class ShadowRenderer::Impl
	{
	public:
		// width = height & 2^n
		const float shadow_tex_length = 1024;

		D3D11_VIEWPORT view_port_old_;
		D3D11_VIEWPORT view_port_shadow_;

		std::unique_ptr<Shader> shadow_map_shader_;

		ConstantBufferShadow1 cb_light_VP_;
		ConstantBufferShadow2 cb_mesh_;

		Impl()
		{
			shadow_map_shader_ = std::make_unique<Shader>();
		}

		bool LoadShader(void)
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> def_element =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			if (!shadow_map_shader_->CompileAndCreateFromFile(Graphics::GetDevice(), "ShadowMap.hlsl", ShaderType::VS, def_element)) return false;
			if (!shadow_map_shader_->CompileAndCreateFromFile(Graphics::GetDevice(), "ShadowMap.hlsl", ShaderType::PS, def_element)) return false;

			return true;
		}
	};

	ShadowRenderer::ShadowRenderer() : impl_(std::make_unique<Impl>()) {}

	ShadowRenderer::~ShadowRenderer() = default;

	bool ShadowRenderer::Initialize(void)
	{
		impl_->LoadShader();

		impl_->cb_light_VP_.light_view = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3(0, 8.f, -8),
			DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3(0, 1, 0));;
		impl_->cb_light_VP_.light_proj = DirectX::SimpleMath::Matrix::CreateOrthographic(20, 15, 2.0f, 16.f);

		impl_->cb_mesh_.world  = DirectX::SimpleMath::Matrix::Identity;

		// create constant buffer
		impl_->shadow_map_shader_->CreateConstantBuffer(Graphics::GetDevice(), impl_->cb_light_VP_);
		impl_->shadow_map_shader_->CreateConstantBuffer(Graphics::GetDevice(), impl_->cb_mesh_);

		// create shadow view port
		impl_->view_port_shadow_.Width = impl_->shadow_tex_length;
		impl_->view_port_shadow_.Height = impl_->shadow_tex_length;
		impl_->view_port_shadow_.MinDepth = 0.0f;
		impl_->view_port_shadow_.MaxDepth = 1.0f;
		impl_->view_port_shadow_.TopLeftX = 0;
		impl_->view_port_shadow_.TopLeftY = 0;

		return true;
	}

	void ShadowRenderer::BeforeRender(void)
	{
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context = Graphics::GetDeviceContext();

		// shadow render target settings
		{
			float clear_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			Graphics::ClearRenderTargetView(RenderTargetType::SHADOW_MAP, clear_color);
			Graphics::SetRenderTarget(RenderTargetType::SHADOW_MAP);

			// default view port
			Graphics::GetViewPort(&impl_->view_port_old_);

			// change shadow view port
			Graphics::SetViewPort(&impl_->view_port_shadow_);
		}

		// settings
		{
			// TODO : shadow view projection update
			impl_->shadow_map_shader_->UpdateConstantBuffer(device_context, impl_->cb_light_VP_, ShaderType::VS, 0, 1);
			impl_->shadow_map_shader_->SetShader(device_context, ShaderType::PS);

			Graphics::SetDepthStencilState(DepthStencilStateType::DEPTH_TEST_ONLY);
			Graphics::SetBlendState(BlendStateType::DISABLED);
			Graphics::SetRasterizerState(RasterizerStateType::CULL_BACK);
		}
	}

	void ShadowRenderer::RenderSetting(void)
	{
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context = Graphics::GetDeviceContext();

		// TODO : world matrix update
		impl_->shadow_map_shader_->UpdateConstantBuffer(device_context, impl_->cb_mesh_, ShaderType::VS, 1, 1);

		impl_->shadow_map_shader_->SetShader(device_context, ShaderType::VS);
	}

	void ShadowRenderer::AfterRender(void)
	{
		Graphics::GetDeviceContext()->RSSetViewports(1, &impl_->view_port_old_);
		Graphics::SetRenderTarget(RenderTargetType::BACK_BUFFER);
	}
}