//
//#include"Player3D.h"
//
//#include"..\Camera.h"
//#include"..\..\Graphics\ConstantBuffer.h"
//#include"..\..\Graphics\Graphics.h"
////#include"..\..\Renderer\SkinnedMesh.h"
//#include"..\..\Graphics\Geometry.h"
//#include"..\..\Graphics\GeometryGenerator.h"
//#include"..\..\Graphics\Shader.h"
//#include"..\..\Graphics\Texture.h"
//#include"..\..\Input\Input.h"
//#include"..\..\Framework\Log.h"
//
//namespace Prizm
//{
//	using PlayerCB = CostantBufferMatrix3DSimple;
//
//	class Player3D::Impl
//	{
//	public:
//		std::unique_ptr<Geometry> geometry_;
//		std::unique_ptr<Shader> shader_;
//		std::unique_ptr<Texture> texture_;
//		//std::unique_ptr<SkinnedMesh> skin_;
//
//		const std::unique_ptr<Graphics>& graphics_;
//
//		Impl(const std::unique_ptr<Graphics>& graphics) : graphics_(graphics), shader_(std::make_unique<Shader>()){}
//	};
//
//	Player3D::Player3D(const std::unique_ptr<Graphics>& graphics) : impl_(std::make_unique<Impl>(graphics))
//	{
//		impl_->texture_ = std::make_unique<Texture>(graphics);
//	}
//	Player3D::~Player3D(void) = default;
//
//	bool Player3D::Init(const std::unique_ptr<Camera>& camera)
//	{
//		/*impl_->skin_ = std::make_unique<SkinnedMesh>(impl_->graphics_);
//		impl_->skin_->ModelLoad("omanko.pzm");
//		impl_->skin_->Initialize("omanko");*/
//
//		impl_->geometry_ = std::make_unique<Geometry>(GeometryGenerator::Cube());
//
//		PlayerCB cb;
//
//		cb.world = DirectX::SimpleMath::Matrix::Identity;
//		cb.view = camera->GetViewMatrix();
//		cb.proj = camera->GetProjectionMatrix();
//
//		impl_->shader_->CreateConstantBuffer<PlayerCB>(impl_->graphics_->GetDevice(), cb);
//
//		LoadShader();
//		
//		impl_->texture_->LoadTexture("wall00.jpg");
//
//		Log::Info("Player3D initialize succeeded.");
//		return true;
//	}
//
//	void Player3D::Run(void)
//	{
//
//	}
//
//	void Player3D::Draw(const std::unique_ptr<Camera>& camera)
//	{
//		DirectX::SimpleMath::Matrix matRotX, matRotY;
//
//		static float Y = 0.0f;
//		Y += 0.02f;
//
//		if (Input::IsTouchPress(Input::max_touchcount - 1))
//		{
//			Y += 10.0f;
//		}
//
//		DirectX::XMStoreFloat4x4(&matRotX, DirectX::XMMatrixRotationX(Y * DirectX::XM_PI / 180.0f));
//		DirectX::XMStoreFloat4x4(&matRotY, DirectX::XMMatrixRotationY(Y * DirectX::XM_PI / 180.0f));
//
//		Prism::CostantBufferMatrix3DSimple cb;
//		cb.world = matRotY * matRotX;
//		cb.view = camera->GetViewMatrix();
//		cb.proj = camera->GetProjectionMatrix();
//
//		Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context = impl_->graphics_->GetDeviceContext();
//
//		impl_->shader_->SetInputLayout(device_context);
//		impl_->shader_->SetShader(device_context, ShaderType::VS);
//		impl_->shader_->SetShader(device_context, ShaderType::PS);
//
//		impl_->graphics_->SetRasterizerState(RasterizerStateType::CULL_BACK);
//		impl_->graphics_->SetDepthStencilState(DepthStencilStateType::DEPTH_WRITE);
//		impl_->graphics_->SetBlendState(BlendStateType::ALIGNMENT_BLEND);
//
//		impl_->texture_->SetPSTexture(0, 1);
//
//		device_context->PSSetSamplers(0, 1, impl_->graphics_->GetSamplerState(SamplerStateType::LINEAR_FILTER_SAMPLER).GetAddressOf());
//
//		impl_->shader_->UpdateConstantBuffer<PlayerCB>(impl_->graphics_->GetDeviceContext(), cb, ShaderType::VS, 0, 1);
//		impl_->geometry_->Draw(impl_->graphics_->GetDeviceContext());
//
//		//impl_->skin_->Draw(camera);
//	}
//
//	bool Player3D::LoadShader(void)
//	{
//		std::vector<D3D11_INPUT_ELEMENT_DESC> def_element =
//		{
//			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "TANGENT"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, 52, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		};
//
//		if (!impl_->shader_->CompileAndCreateFromFile(impl_->graphics_->GetDevice(), "Texture.hlsl", ShaderType::VS, def_element)) return false;
//		if (!impl_->shader_->CompileAndCreateFromFile(impl_->graphics_->GetDevice(), "Texture.hlsl", ShaderType::PS, def_element)) return false;
//
//		return true;
//	}
//}