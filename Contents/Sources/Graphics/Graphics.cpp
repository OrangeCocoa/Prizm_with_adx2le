
#include<string>
#include<vector>
#include<DirectXTK/SimpleMath.h>

#include"Graphics.h"
#include"ConstantBuffer.h"
#include"Geometry.h"
#include"Shader.h"
#include"Texture.h"
#include"..\Framework\ResourcePool.h"
#include"..\Framework\Utils.h"
#include"..\Framework\Log.h"
#include"..\Window\Window.h"

#pragma comment(lib, "Window.lib")
#pragma comment(lib, "Framework.lib")

namespace Prizm
{
	namespace Graphics
	{
		Microsoft::WRL::ComPtr<ID3D11Device>								device_;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>							device_context_; // immediate
		Microsoft::WRL::ComPtr<IDXGISwapChain>								swap_chain_;

		std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>			render_targets_;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>					shadow_resource_;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>						depth_stencil_view_;

		std::vector<Microsoft::WRL::ComPtr<ID3D11BlendState>>				blend_states_;
		std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>>				sampler_states_;
		std::vector<Microsoft::WRL::ComPtr<ID3D11RasterizerState>>			rasterizer_states_;
		std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilState>>		depth_stencil_states_;

		D3D11_VIEWPORT														view_port_;

		Microsoft::WRL::ComPtr<IDXGIAdapter>								adapter_;
		Microsoft::WRL::ComPtr<IDXGIFactory>								factory_;
#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D11Debug>									debug_;
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation>					annotation_;
#endif
		std::vector<DXGI_MODE_DESC>											display_mode_list_;

		bool				vsync_enabled_;
		int					vram_;
		char				gpu_description_[128];
		HWND				hwnd_;
		D3D_FEATURE_LEVEL	feature_level_;
		DXGI_SAMPLE_DESC	sample_desc_;			// MSAA sample desc

		unsigned int window_width_, window_height_;
		unsigned int numerator_, denominator_;
		bool		 fullscreen_;

		void MSAASampleCheck(void)
		{
			sample_desc_ = {};
			for (int i = 1; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i <<= 1)
			{
				UINT Quality;
				if (succeeded(device_->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &Quality)))
				{
					if (0 < Quality)
					{
						sample_desc_.Count = i;
						sample_desc_.Quality = Quality - 1;
					}
				}
			}
		}

		bool CreateAdapter(void)
		{
			std::string err_msg = "";

			if (failed(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory_.GetAddressOf()))))
			{
				err_msg =
					"Do not create ""DXGIFactory"".";
			}
			else
			{
				// Use the factory to create an adapter for the primary graphics interface (video card).
				if (failed(factory_->EnumAdapters(0, adapter_.GetAddressOf())))
				{
					err_msg =
						"Do not create an adapter for the primary graphics interface.";
				}
			}

			if (!err_msg.empty())
			{
				Log::Error(err_msg + " (Graphics.cpp)");
				return false;
			}

			return true;
		}

		bool GetDisplayMode(void)
		{
			std::vector<DXGI_MODE_DESC> desc;
			Microsoft::WRL::ComPtr<IDXGIOutput> adapter_output;
			unsigned num_modes;
			std::string err_msg = "";

			if (failed(CreateAdapter()))	return false;

			// Enumerate the primary adapter output (monitor).
			if (failed(adapter_->EnumOutputs(0, adapter_output.GetAddressOf())))
			{
				err_msg =
					"Do not output enumerate the primary adapter.";
			}
			else
			{
				// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
				if (failed(adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, nullptr)))
				{
					err_msg =
						"Do not get the number of modes the ""DXGI_FORMAT_R8G8B8A8_UNORM"".";
				}

				desc.resize(num_modes);

				if (failed(adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, desc.data())))
				{
					err_msg =
						"Do not fit the number of modes the ""DXGI_FORMAT_R8G8B8A8_UNORM"" to desc.";
				}
			}

			if (!err_msg.empty())
			{
				Log::Error(err_msg + " (Graphics.cpp)");
				return false;
			}

			for (unsigned i = 0; i < num_modes; ++i)
			{
				display_mode_list_.emplace_back(desc[i]);

				if (display_mode_list_[i].Width == (unsigned int)window_width_)
				{
					if (display_mode_list_[i].Height == (unsigned int)window_height_)
					{
						numerator_ = display_mode_list_[i].RefreshRate.Numerator;
						denominator_ = display_mode_list_[i].RefreshRate.Denominator;
					}
				}
			}

			if (numerator_ == 0 && denominator_ == 0)
			{
				numerator_ = display_mode_list_[num_modes / 2].RefreshRate.Numerator;
				denominator_ = display_mode_list_[num_modes / 2].RefreshRate.Denominator;
				window_width_ = display_mode_list_[num_modes / 2].Width;
				window_height_ = display_mode_list_[num_modes / 2].Height;
			}

			// Get the adapter (video card) description.
			DXGI_ADAPTER_DESC adapter_desc;

			if (failed(adapter_->GetDesc(&adapter_desc)))
			{
				err_msg =
					"Failed function -> adapter->GetDesc()\nGet the video card description failed.";
				return false;
			}
			else
			{
				// Store the dedicated video card memory in megabytes.
				vram_ = (int)(adapter_desc.DedicatedVideoMemory / 1024 / 1024);

				// Convert the name of the video card to a character array and store it.
				size_t stringLength;

				if (wcstombs_s(&stringLength, gpu_description_, 128, adapter_desc.Description, 128) != 0)
				{
					err_msg =
						"Failed function -> wcstombs_s()\nCannot convert the name of the video card to a character array and store it.";
					return false;
				}
			}

			if (!err_msg.empty())
			{
				MessageBoxA(nullptr, err_msg.c_str(), "Error Notification", MB_OK);
				return false;
			}

			//Release
			desc.clear();
			adapter_output.Reset();

			return true;
		}

		bool InitDeviceAndSwapChain(void)
		{// CreateDevice and CreateSwapChain separated because MSAA sample check.
			UINT debug_flag = 0;
#ifdef _DEBUG
			debug_flag = D3D11_CREATE_DEVICE_DEBUG;
#endif
			feature_level_ = D3D_FEATURE_LEVEL_11_1;

			D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				debug_flag,
				&feature_level_,
				1,
				D3D11_SDK_VERSION,
				device_.GetAddressOf(),
				nullptr,
				device_context_.GetAddressOf());

			sample_desc_.Count = 1;
			sample_desc_.Quality = 0;

			// Forward MSAA is criticaly 
			//MSAASampleCheck();

			DXGI_SWAP_CHAIN_DESC swap_chain_desc;
			memset(&swap_chain_desc, 0, sizeof(swap_chain_desc));

			swap_chain_desc.BufferCount = 3;

			swap_chain_desc.BufferDesc.Width = window_width<int>;
			swap_chain_desc.BufferDesc.Height = window_height<int>;
			swap_chain_desc.BufferDesc.Format = static_cast<DXGI_FORMAT>(ImageFormat::RGBA8UN);

			if (vsync_enabled_)
			{	// Set the refresh rate of the back buffer.
				swap_chain_desc.BufferDesc.RefreshRate.Numerator = numerator_;
				swap_chain_desc.BufferDesc.RefreshRate.Denominator = denominator_;
			}
			else
			{
				swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
				swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
			}

			swap_chain_desc.OutputWindow = hwnd_;
			swap_chain_desc.Windowed = !fullscreen_;
			swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
			swap_chain_desc.SampleDesc = sample_desc_;
			swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swap_chain_desc.Flags = 0;

			// Create the swap chain.
			HRESULT result = factory_->CreateSwapChain(
				device_.Get(),
				&swap_chain_desc,
				swap_chain_.GetAddressOf());

			if (failed(result))
			{
				Log::Error("Failed CreateDeviceAndSwapChain. (Graphics.cpp)");
				return false;
			}

#ifdef _DEBUG
			// Direct3D SDK Debug Layer
			//------------------------------------------------------------------------------------------
			// src1: https://blogs.msdn.microsoft.com/chuckw/2012/11/30/direct3d-sdk-debug-layer-tricks/
			// src2: http://seanmiddleditch.com/direct3d-11-debug-api-tricks/
			//------------------------------------------------------------------------------------------
			device_->QueryInterface<ID3D11Debug>(&debug_);
			if (debug_)
			{
				Log::Info("Succeed QueryInterface for ID3D11Debug.");

				Microsoft::WRL::ComPtr<ID3D11InfoQueue> info_queue;
				debug_->QueryInterface<ID3D11InfoQueue>(&info_queue);

				if (info_queue)
				{
					Log::Info("Succeed QueryInterface for ID3D11InfoQueue.");

					info_queue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
					info_queue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);

					D3D11_MESSAGE_ID hide[] =
					{
						D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET,
						// Add more message IDs here as needed
					};

					D3D11_INFO_QUEUE_FILTER filter;
					memset(&filter, 0, sizeof(filter));
					filter.DenyList.NumIDs = _countof(hide);
					filter.DenyList.pIDList = hide;
					info_queue->AddStorageFilterEntries(&filter);
					info_queue->Release();
				}
			}

			if (failed(device_context_->QueryInterface<ID3DUserDefinedAnnotation>(&annotation_)))
			{
				Log::Error("Can't QueryInterface(ID3DUserDefinedAnnotation). (Graphics.cpp)");
				return false;
			}
#endif
			return true;
		}

		bool CreateDefaultRenderTargetView(void)
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> tex_2d;

			swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), static_cast<void**>(&tex_2d));

			device_->CreateRenderTargetView(tex_2d.Get(), nullptr, render_targets_[RenderTargetType::BACK_BUFFER].GetAddressOf());

			return true;
		}

		bool CreateShadowRenderTargetView(void)
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_2d;

			D3D11_TEXTURE2D_DESC desc = {};
			desc.Width = window_width<unsigned int>;
			desc.Height = window_height<unsigned int>;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			//device_->CreateTexture2D(&desc, nullptr, texture_2d.GetAddressOf());

			desc.Format = static_cast<DXGI_FORMAT>(ImageFormat::R32F);
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			device_->CreateTexture2D(&desc, nullptr, texture_2d.GetAddressOf());

			D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};
			RTVDesc.Format = desc.Format;
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			RTVDesc.Texture2D.MipSlice = 0;
			device_->CreateRenderTargetView(texture_2d.Get(), &RTVDesc, render_targets_[RenderTargetType::SHADOW_MAP].GetAddressOf());

			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
			SRVDesc.Format = desc.Format;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;
			device_->CreateShaderResourceView(texture_2d.Get(), &SRVDesc, shadow_resource_.GetAddressOf());

			return true;
		}

		bool CreateRenderTargetView(void)
		{
			for (unsigned int i = 0; i < RenderTargetType::RENDER_TARGET_MAX; ++i)
			{
				render_targets_.emplace_back();
			}

			CreateDefaultRenderTargetView();

			CreateShadowRenderTargetView();

			return true;
		}

		bool CreateDepthStencilView(void)
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> depth;
			D3D11_TEXTURE2D_DESC depth_desc;
			memset(&depth_desc, 0, sizeof(depth_desc));
			depth_desc.Width = window_width<int>;
			depth_desc.Height = window_height<int>;
			depth_desc.ArraySize = 1;
			depth_desc.MipLevels = 1;
			depth_desc.Format = static_cast<DXGI_FORMAT>(ImageFormat::D24UNORM_S8U);
			depth_desc.Usage = static_cast<D3D11_USAGE>(BufferUsage::STATIC_RW);
			depth_desc.SampleDesc = sample_desc_;
			depth_desc.BindFlags = static_cast<D3D11_BIND_FLAG>(TextureUsage::DEPTH_TARGET);
			depth_desc.CPUAccessFlags = 0;
			depth_desc.MiscFlags = 0;

			device_->CreateTexture2D(&depth_desc, nullptr, depth.GetAddressOf());

			// depth stencil view and shader resource view for the shadow map (^ BindFlags)
			D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
			memset(&dsv_desc, 0, sizeof(dsv_desc));
			dsv_desc.Format = depth_desc.Format;
			dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			dsv_desc.Texture2D.MipSlice = 0;

			if (failed(device_->CreateDepthStencilView(depth.Get(), &dsv_desc, depth_stencil_view_.GetAddressOf())))
			{
				Log::Error("Cannot add depth Stencil Target View. (Graphics.cpp)");
				return false;
			}

			return true;
		}

		bool CreateViewPort(void)
		{
			// viewport set to immediate device context
			view_port_.Width = window_width<float>;
			view_port_.Height = window_height<float>;
			view_port_.MinDepth = 0.0f;
			view_port_.MaxDepth = 1.0f;
			view_port_.TopLeftX = 0;
			view_port_.TopLeftY = 0;
			device_context_->RSSetViewports(1, &view_port_);

			return true;
		}

		bool CreateDefaultRasterizerState(void)
		{
			const std::string err("Unable to create Rasterizer State: Cull ");

			// MSDN: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476198(v=vs.85).aspx
			D3D11_RASTERIZER_DESC rs_desc = {};

			rs_desc.FillMode = D3D11_FILL_WIREFRAME;
			rs_desc.CullMode = D3D11_CULL_NONE;
			rs_desc.DepthClipEnable = true;
			rs_desc.MultisampleEnable = true;
			rs_desc.FrontCounterClockwise = false;
			rs_desc.DepthBias = 0;
			rs_desc.ScissorEnable = false;
			rs_desc.DepthBiasClamp = 0;
			rs_desc.SlopeScaledDepthBias = 0.0f;
			rs_desc.AntialiasedLineEnable = true;

			if (failed(device_->CreateRasterizerState(&rs_desc, &rasterizer_states_[static_cast<int>(RasterizerStateType::WIRE_FRAME)])))
			{
				Log::Error(err + "WireFrame\n");
				return false;
			}

			rs_desc.FillMode = D3D11_FILL_SOLID;

			if (failed(device_->CreateRasterizerState(&rs_desc, &rasterizer_states_[static_cast<int>(RasterizerStateType::CULL_NONE)])))
			{
				Log::Error(err + "None\n");
				return false;
			}

			rs_desc.CullMode = D3D11_CULL_BACK;

			if (failed(device_->CreateRasterizerState(&rs_desc, &rasterizer_states_[static_cast<int>(RasterizerStateType::CULL_BACK)])))
			{
				Log::Error(err + "Back\n");
				return false;
			}

			rs_desc.CullMode = D3D11_CULL_FRONT;
			if (failed(device_->CreateRasterizerState(&rs_desc, &rasterizer_states_[static_cast<int>(RasterizerStateType::CULL_FRONT)])))
			{
				Log::Error(err + "Front\n");
				return false;
			}

			return true;
		}

		bool CreateDefaultBlendState(void)
		{
			const std::string err("Unable to create Blend State: ");

			D3D11_RENDER_TARGET_BLEND_DESC rt_blend_desc = {};
			rt_blend_desc.BlendEnable = true;
			rt_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
			rt_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_MIN;
			rt_blend_desc.DestBlend = D3D11_BLEND_ONE;
			rt_blend_desc.DestBlendAlpha = D3D11_BLEND_ONE;
			rt_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			rt_blend_desc.SrcBlend = D3D11_BLEND_ONE;
			rt_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;

			D3D11_BLEND_DESC desc = {};
			desc.RenderTarget[0] = rt_blend_desc;

			if (failed(device_->CreateBlendState(&desc, &(blend_states_[BlendStateType::ADDITIVE_COLOR]))))
			{
				Log::Error(err + "Additive Color\n");
				return false;
			}

			rt_blend_desc.BlendEnable = true;
			rt_blend_desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			rt_blend_desc.DestBlend = D3D11_BLEND_ONE;
			rt_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
			rt_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
			rt_blend_desc.DestBlendAlpha = D3D11_BLEND_ZERO;
			rt_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rt_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			desc.RenderTarget[0] = rt_blend_desc;
			if (failed(device_->CreateBlendState(&desc, &(blend_states_[BlendStateType::ALPHA_BLEND]))))
			{
				Log::Error(err + "Alpha Blend\n");
				return false;
			}

			rt_blend_desc.BlendEnable = true;
			rt_blend_desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			rt_blend_desc.DestBlend = D3D11_BLEND_ONE;
			rt_blend_desc.BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
			rt_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
			rt_blend_desc.DestBlendAlpha = D3D11_BLEND_ZERO;
			rt_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rt_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			desc.RenderTarget[0] = rt_blend_desc;
			if (failed(device_->CreateBlendState(&desc, &(blend_states_[BlendStateType::SUBTRACT_BLEND]))))
			{
				Log::Error(err + "Subtract Blend\n");
				return false;
			}

			rt_blend_desc.BlendEnable = true;
			rt_blend_desc.SrcBlend = D3D11_BLEND_ZERO;
			rt_blend_desc.DestBlend = D3D11_BLEND_SRC_COLOR;
			rt_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
			rt_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
			rt_blend_desc.DestBlendAlpha = D3D11_BLEND_ZERO;
			rt_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rt_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			desc.RenderTarget[0] = rt_blend_desc;
			if (failed(device_->CreateBlendState(&desc, &(blend_states_[BlendStateType::MULTIPLE_BLEND]))))
			{
				Log::Error(err + "Multiple Blend\n");
				return false;
			}

			rt_blend_desc.BlendEnable = true;
			rt_blend_desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			rt_blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			rt_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
			rt_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
			rt_blend_desc.DestBlendAlpha = D3D11_BLEND_ZERO;
			rt_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rt_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			desc.RenderTarget[0] = rt_blend_desc;
			if (failed(device_->CreateBlendState(&desc, &(blend_states_[BlendStateType::ALIGNMENT_BLEND]))))
			{
				Log::Error(err + "Alignment Blend\n");
				return false;
			}

			rt_blend_desc.BlendEnable = false;
			rt_blend_desc.SrcBlend = D3D11_BLEND_ONE;
			rt_blend_desc.DestBlend = D3D11_BLEND_ZERO;
			rt_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
			rt_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
			rt_blend_desc.DestBlendAlpha = D3D11_BLEND_ZERO;
			rt_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rt_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			desc.RenderTarget[0] = rt_blend_desc;
			if (failed(device_->CreateBlendState(&desc, &(blend_states_[BlendStateType::DISABLED]))))
			{
				Log::Error(err + "Disabled\n");
				return false;
			}
			return true;
		}

		bool CreateDefaultSamplerState(void)
		{
			const std::string err("Unable to create Sampler State: ");

			D3D11_SAMPLER_DESC sampler_desc = {};
			sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
			sampler_desc.MaxAnisotropy = 16;
			sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			//::memcpy(sampler_desc.BorderColor, 0, sizeof(float) * 4);
			sampler_desc.MinLOD = 0;
			sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
			sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			if (failed(device_->CreateSamplerState(&sampler_desc, &(sampler_states_[SamplerStateType::WRAP_SAMPLER]))))
			{
				Log::Error(err + "Wrap\n");
				return false;
			}

			sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			if (failed(device_->CreateSamplerState(&sampler_desc, &(sampler_states_[SamplerStateType::POINT_SAMPLER]))))
			{
				Log::Error(err + "Point\n");
				return false;
			}

			sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			if (failed(device_->CreateSamplerState(&sampler_desc, &(sampler_states_[SamplerStateType::LINEAR_FILTER_SAMPLER_WRAP_UVW]))))
			{
				Log::Error(err + "Linear Wrap\n");
				return false;
			}

			sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			if (failed(device_->CreateSamplerState(&sampler_desc, &(sampler_states_[SamplerStateType::LINEAR_FILTER_SAMPLER]))))
			{
				Log::Error(err + "Linear\n");
				return false;
			}

			return true;
		}

		bool CreateDefaultDepthStencilState(void)
		{
			const std::string err("Unable to create Depth Stencil State: ");

			D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};

			// Set up the description of the stencil state.
			depth_stencil_desc.DepthEnable = true;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

			depth_stencil_desc.StencilEnable = true;
			depth_stencil_desc.StencilReadMask = 0xFF;
			depth_stencil_desc.StencilWriteMask = 0xFF;

			// Stencil operations if pixel is front-facing.
			depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// Stencil operations if pixel is back-facing.
			depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_LESS;

			// Create the depth stencil states.
			if (failed(device_->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states_[DepthStencilStateType::DEPTH_STENCIL_WRITE])))
			{
				Log::Error(err + "D S Write\n");
				return false;
			}

			depth_stencil_desc.DepthEnable = false;
			depth_stencil_desc.StencilEnable = false;
			if (failed(device_->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states_[DepthStencilStateType::DEPTH_STENCIL_DISABLED])))
			{
				Log::Error(err + "D S Disabled\n");
				return false;
			}

			depth_stencil_desc.DepthEnable = true;
			depth_stencil_desc.StencilEnable = false;
			if (failed(device_->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states_[DepthStencilStateType::DEPTH_WRITE])))
			{
				Log::Error(err + "Depth Write\n");
				return false;
			}

			depth_stencil_desc.DepthEnable = false;
			depth_stencil_desc.StencilEnable = true;
			if (failed(device_->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states_[DepthStencilStateType::STENCIL_WRITE])))
			{
				Log::Error(err + "Stencil Write\n");
				return false;
			}

			depth_stencil_desc.DepthEnable = true;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depth_stencil_desc.StencilEnable = true;
			if (failed(device_->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states_[DepthStencilStateType::DEPTH_TEST_ONLY])))
			{
				Log::Error(err + "Write\n");
				return false;
			}

			return true;
		}

		void ReportLiveObjects(const std::string& log_message)
		{
#ifdef _DEBUG
			if (!log_message.empty())
			{
				Log::Info(log_message);
			}

			if (succeeded(device_->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(debug_.GetAddressOf()))))
			{
				debug_->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			}
#endif
		}

		bool ChangeWindowSize(UINT Width, UINT Height)
		{
			if (!swap_chain_) return false;

			DXGI_SWAP_CHAIN_DESC desc;
			swap_chain_->GetDesc(&desc);

			device_context_->OMSetRenderTargets(0, nullptr, nullptr);

			Log::Info("Release back render target.");

			if (failed(swap_chain_->ResizeBuffers(desc.BufferCount, Width, Height, desc.BufferDesc.Format, desc.Flags))) return false;

			if (!CreateRenderTargetView()) return false;

			if (!CreateDepthStencilView()) return false;

			device_context_->OMSetRenderTargets(1, render_targets_[RenderTargetType::BACK_BUFFER].GetAddressOf(), depth_stencil_view_.Get());

			return true;
		}

		bool Initialize(int width, int height, const bool vsync, HWND hwnd, const bool FULL_SCREEN)
		{
			vsync_enabled_ = false;
			fullscreen_ = false;
			window_width_ = window_height_ = numerator_ = denominator_ = 0;

			for (int i = 0; i < static_cast<int>(RasterizerStateType::RASTERIZER_STATE_MAX); ++i)
			{
				rasterizer_states_.emplace_back();
			}

			for (int i = 0; i < static_cast<int>(BlendStateType::BLEND_STATE_MAX); ++i)
			{
				blend_states_.emplace_back();
			}

			for (int i = 0; i < static_cast<int>(DepthStencilStateType::DEPTH_STENCIL_STATE_MAX); ++i)
			{
				depth_stencil_states_.emplace_back();
			}

			for (int i = 0; i < static_cast<int>(SamplerStateType::DEFAULT_SAMPLER_MAX); ++i)
			{
				sampler_states_.emplace_back();
			}

			hwnd_ = hwnd;
			window_width_ = width;
			window_height_ = height;

			vsync_enabled_ = vsync;
			fullscreen_ = FULL_SCREEN;

			if (!GetDisplayMode()) return false;

			if (!InitDeviceAndSwapChain()) return false;

			if (!CreateRenderTargetView()) return false;

			if (!CreateDepthStencilView()) return false;

			// RenderTarget set to immediate device context
			device_context_->OMSetRenderTargets(1, render_targets_[RenderTargetType::BACK_BUFFER].GetAddressOf(), depth_stencil_view_.Get());

			if (!CreateViewPort()) return false;

			if (!CreateDefaultRasterizerState()) return false;

			if (!CreateDefaultBlendState()) return false;

			if (!CreateDefaultSamplerState()) return false;

			if (!CreateDefaultDepthStencilState()) return false;

			Geometry::SetDevice(device_);

			Log::Info("Graphics initialize succeeded.\n\nDirectX11 is all grean!\n");

			return true;
		}

		void Finalize(void)
		{
			ReportLiveObjects("Finalize call.");

			if (swap_chain_)
			{
				swap_chain_->SetFullscreenState(false, nullptr);
				swap_chain_.Reset();
			}

			if (device_context_)
			{
				device_context_.Reset();
			}

			if (device_)
			{
				device_.Reset();
			}

			if (adapter_)
			{
				adapter_.Reset();
			}

			if (factory_)
			{
				factory_.Reset();
			}

#ifdef _DEBUG
			if (debug_)
			{
				debug_.Reset();
			}

			if (annotation_)
			{
				annotation_->EndEvent();
				annotation_.Reset();
			}
#endif
			return;
		}

		void BeginFrame(void)
		{
			float clear_color[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red, green, blue, alpha
			device_context_->ClearRenderTargetView(render_targets_[RenderTargetType::BACK_BUFFER].Get(), clear_color);
			device_context_->ClearDepthStencilView(depth_stencil_view_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}

		void EndFrame()
		{
			swap_chain_->Present(static_cast<unsigned>(vsync_enabled_), 0);

			device_context_->IASetInputLayout(nullptr);
			device_context_->VSSetShader(nullptr, nullptr, 0);
			device_context_->GSSetShader(nullptr, nullptr, 0);
			device_context_->HSSetShader(nullptr, nullptr, 0);
			device_context_->DSSetShader(nullptr, nullptr, 0);
			device_context_->PSSetShader(nullptr, nullptr, 0);
			device_context_->CSSetShader(nullptr, nullptr, 0);
		}

		bool ChangeWindowMode(void)
		{
			if (!swap_chain_) return false;

			DXGI_SWAP_CHAIN_DESC desc;
			swap_chain_->GetDesc(&desc);

			int full_screen;

			// GetFullscreenState
			swap_chain_->GetFullscreenState(&full_screen, NULL);

			// SetFullscreenState
			swap_chain_->SetFullscreenState(!full_screen, NULL);

			switch (full_screen)
			{
			case TRUE:
				Log::Info("Change full screen.");
				break;
			case FALSE:
				Log::Info("Change window screen.");
				break;
			}

			::ShowWindow(hwnd_, SW_SHOW);

			if (!ChangeWindowSize(0, 0)) return false;

			return true;
		}

		void SetRenderTarget(RenderTargetType type)
		{
			device_context_->OMSetRenderTargets(1, render_targets_[type].GetAddressOf(), depth_stencil_view_.Get());
		}

		void ClearRenderTargetView(RenderTargetType type, const float * clear_color)
		{
			device_context_->ClearRenderTargetView(render_targets_[type].Get(), clear_color);
		}

		void ClearDepthStencilView(void)
		{
			device_context_->ClearDepthStencilView(depth_stencil_view_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}

		void GetViewPort(D3D11_VIEWPORT* vp)
		{
			UINT	VP_num = 1;
			device_context_->RSGetViewports(&VP_num, vp);
		}

		void SetViewPort(D3D11_VIEWPORT* vp)
		{
			device_context_->RSSetViewports(1, vp);
		}

		void SetBlendState(BlendStateType type)
		{
			float blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
			device_context_->OMSetBlendState(blend_states_[type].Get(), blendFactor, 0xffffffff);
		}

		void SetRasterizerState(RasterizerStateType type)
		{
			device_context_->RSSetState(rasterizer_states_[type].Get());
		}

		void SetDepthStencilState(DepthStencilStateType type)
		{
			device_context_->OMSetDepthStencilState(depth_stencil_states_[type].Get(), 0);
		}

		Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState(SamplerStateType type)
		{
			return sampler_states_[type];
		}

		void SetSamplerState(unsigned int shader_type, SamplerStateType ss_type, unsigned int register_slot)
		{
			switch (shader_type)
			{
			case ShaderType::VS:
				device_context_->VSSetSamplers(register_slot, 1, &sampler_states_[ss_type]);
				break;
			case ShaderType::PS:
				device_context_->PSSetSamplers(register_slot, 1, &sampler_states_[ss_type]);
				break;
			case ShaderType::GS:
				device_context_->GSSetSamplers(register_slot, 1, &sampler_states_[ss_type]);
				break;
			case ShaderType::HS:
				device_context_->HSSetSamplers(register_slot, 1, &sampler_states_[ss_type]);
				break;
			case ShaderType::DS:
				device_context_->DSSetSamplers(register_slot, 1, &sampler_states_[ss_type]);
				break;
			case ShaderType::CS:
				device_context_->CSSetSamplers(register_slot, 1, &sampler_states_[ss_type]);
				break;
			}
		}

		Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice(void) { return device_; }

		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetDeviceContext(void) { return device_context_; }

		HWND GetWindowHandle(void) { return hwnd_; }
	}
}