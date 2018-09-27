
#include"Geometry.h"

namespace Prizm
{
	Microsoft::WRL::ComPtr<ID3D11Device> Geometry::device_ = nullptr;

	void Geometry::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& dc)
	{
		UINT offset[] = { 0 };
		dc->IASetVertexBuffers(0, 1, vertex_buffer_.buffer_data.GetAddressOf(), &vertex_buffer_.desc.stride, offset);

		dc->IASetIndexBuffer(index_buffer_.buffer_data.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(topology_));

		dc->DrawIndexed(index_buffer_.desc.element_count, 0, 0);
	}

	void Geometry::CleanUp(void)
	{
		vertex_buffer_.CleanUp();
		index_buffer_.CleanUp();
	}

	void Geometry::SetDevice(Microsoft::WRL::ComPtr<ID3D11Device>& device)
	{
		device_ = device;
	}
}