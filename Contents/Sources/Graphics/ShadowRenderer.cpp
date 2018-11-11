
#include"ShadowRenderer.h"
#include"Graphics.h"
#include"Shader.h"

namespace Prizm
{
	class ShadowRenderer::Impl
	{
	public:
		D3D11_VIEWPORT view_port_old_;
		D3D11_VIEWPORT view_port_;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTV_old_;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTV_;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSV_old_;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSV_;


		std::unique_ptr<Shader> shadow_map_shader_;

		Impl()
		{
			shadow_map_shader_ = std::make_unique<Shader>();
		}

		bool LoadShader(void)
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> def_element =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

		return true;
	}
}