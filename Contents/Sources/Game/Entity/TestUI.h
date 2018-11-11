#pragma once

#include"..\..\Renderer\Mesh.h"
#include"..\..\Renderer\GeometryGenerator.h"
#include"..\..\Renderer\Texture.h"
#include"..\..\Renderer\Shader.h"

/*
texture animation is cutting uv for constant buffer
*/

namespace Prizm
{
	class UI
	{
	private:
		std::unique_ptr<Mesh> mesh_;
		std::unique_ptr<Shader> shader_;
		std::unique_ptr<Texture> texture_;

		const std::unique_ptr<Graphics>& graphics_;

	public:
		TestUI(const std::unique_ptr<Graphics>& graphics) : graphics_(graphics), shader_(std::make_unique<Shader>()) { texture_ = std::make_unique<Texture>(graphics); }
		~TestUI(){}

		bool Init(void)
		{
			mesh_ = std::make_unique<Mesh>(PrimitiveGeometry::Quad2D(1, 1));

			LoadShader();

			texture_->LoadTexture("wall00.jpg");

			return true;
		}

		void Run(void)
		{
			
		}

		void Draw(void)
		{
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context = graphics_->GetDeviceContext();

			shader_->SetInputLayout(device_context);
			shader_->SetVertexShader(device_context);
			shader_->SetPixelShader(device_context);

			graphics_->SetRasterizerState(DefaultRasterizerState::CULL_NONE);
			graphics_->SetDepthStencilState(DefaultDepthStencilState::DEPTH_STENCIL_DISABLED);
			graphics_->SetBlendState(DefaultBlendState::ALIGNMENT_BLEND);

			texture_->SetPSTexture(0, 1);

			device_context->PSSetSamplers(0, 1, graphics_->GetSamplerState(DefaultSamplerState::LINEAR_FILTER_SAMPLER).GetAddressOf());

			mesh_->Draw(graphics_->GetDeviceContext());
		}

		bool LoadShader(void)
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> def_element =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			if (!shader_->CompileAndCreateFromFile(graphics_->GetDevice(), "DefaultUI.hlsl", ShaderType::VS, def_element)) return false;
			if (!shader_->CompileAndCreateFromFile(graphics_->GetDevice(), "DefaultUI.hlsl", ShaderType::PS, def_element)) return false;
			return true;
		}
	};
}