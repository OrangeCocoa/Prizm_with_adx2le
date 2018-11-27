
#include"Geometry.h"

namespace Prizm
{
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
	void Geometry::MovePosition2DScreenToRatio(float x, float y)
	{
		for (auto& vtx : vertex_2D_)
		{
			vtx.position.x += x / (window_width<float> / 2);
			vtx.position.y += y / (window_height<float> / 2);
		}

		vertex_buffer_.Update(Graphics::GetDeviceContext(), static_cast<const void*>(vertex_2D_.data()));
	}
}