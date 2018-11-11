#pragma once

#include<memory>

namespace Prizm
{
	using SwapChain = Microsoft::WRL::ComPtr<IDXGISwapChain>;
	using DeviceContext = Microsoft::WRL::ComPtr<ID3D11DeviceContext>;

	class RenderTarget
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;
	public:
		RenderTarget();
		~RenderTarget();

		bool CreateBackBuffer(SwapChain&, DeviceContext&);
		bool CreateShadow(DeviceContext&);

		ID3D11RenderTargetView* GetRTV();
		ID3D11ShaderResourceView* GetSRV();
	};
}