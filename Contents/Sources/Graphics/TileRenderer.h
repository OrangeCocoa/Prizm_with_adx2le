#pragma once

#include<memory>
#include<wrl\client.h>
#include<d3d11_4.h>

namespace Prizm
{
	using Device = Microsoft::WRL::ComPtr<ID3D11Device>;
	using DeviceContext = Microsoft::WRL::ComPtr<ID3D11DeviceContext>;

	class TileRenderer
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		TileRenderer();
		~TileRenderer();

		void Initialize(Device);

		void DispatchForward(DeviceContext);
	};
}