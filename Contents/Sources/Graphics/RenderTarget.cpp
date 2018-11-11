
#include<wrl\client.h>
#include<d3d11_4.h>

#include"RenderTarget.h"
#include"..\Window\Window.h"

namespace Prizm
{
	class RenderTarget::Impl
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTV_;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV_;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex_;
	};

	RenderTarget::RenderTarget() : impl_(std::make_unique<Impl>()){}

	RenderTarget::~RenderTarget(){}

	bool RenderTarget::CreateBackBuffer(SwapChain& swap_chain, DeviceContext& device_comtext)
	{
		Microsoft::WRL::ComPtr<ID3D11Device> device;

		device_comtext->GetDevice(device.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex_2d;

		swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), static_cast<void**>(&tex_2d));

		device->CreateRenderTargetView(tex_2d.Get(), nullptr, impl_->RTV_.GetAddressOf());

		return true;
	}

	bool RenderTarget::CreateShadow(DeviceContext& device_context)
	{
		Microsoft::WRL::ComPtr<ID3D11Device> device;

		device_context->GetDevice(device.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_2d;

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = window_width<unsigned int>;
		desc.Height = window_height<unsigned int>;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		device->CreateTexture2D(&desc, nullptr, texture_2d.GetAddressOf());

		desc.Format = DXGI_FORMAT_R32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		device->CreateTexture2D(&desc, nullptr, texture_2d.GetAddressOf());

		RTVDesc.Format = desc.Format;
		RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;
		device->CreateRenderTargetView(texture_2d.Get(), &RTVDesc, impl_->RTV_.GetAddressOf());

		SRVDesc.Format = desc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(texture_2d.Get(), &SRVDesc, impl_->SRV_.GetAddressOf());

		return true;
	}

	ID3D11RenderTargetView* RenderTarget::GetRTV()
	{
		return impl_->RTV_.Get();
	}

	ID3D11ShaderResourceView * RenderTarget::GetSRV()
	{
		return impl_->SRV_.Get();
	}
}