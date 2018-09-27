#pragma once

#include<vector>
#include<wrl\client.h>
#include<DirectXTK/SimpleMath.h>

#include"Buffer.h"
#include"Texture.h"
#include"ConstantBuffer.h"

namespace Prizm
{
	enum class TopologyType
	{
		POINT_LIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		TRIANGLE_LIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TRIANGLE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		LINE_LIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		LINE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,

		TOPOLOGY_MAX
	};

	class Geometry
	{
	private:
		static Microsoft::WRL::ComPtr<ID3D11Device> device_;
		Buffer	vertex_buffer_;
		Buffer	index_buffer_;
		Buffer	constant_buffer_;
		TopologyType topology_;
		std::vector<DefaultVertexBufferData2D> vertex_2D_;

	public:
		template<class VertexBufferType>
		Geometry(const std::vector<VertexBufferType>& vertices, const std::vector<unsigned>& indices, TopologyType topology)
		{
			BufferDesc buffer_desc = {};

			buffer_desc.type = BufferType::VERTEX_BUFFER;
			buffer_desc.usage = BufferUsage::STATIC_RW;
			buffer_desc.element_count = static_cast<unsigned>(vertices.size());
			buffer_desc.stride = sizeof(vertices[0]);

			vertex_buffer_ = Buffer(buffer_desc);
			vertex_buffer_.Initialize(device_.Get(), static_cast<const void*>(vertices.data()));

			buffer_desc.type = BufferType::INDEX_BUFFER;
			buffer_desc.usage = BufferUsage::STATIC_RW;
			buffer_desc.element_count = static_cast<unsigned>(indices.size());
			buffer_desc.stride = sizeof(unsigned);

			index_buffer_ = Buffer(buffer_desc);
			index_buffer_.Initialize(device_.Get(), static_cast<const void*>(indices.data()));
		}
		
		Geometry(const std::vector<DefaultVertexBufferData2D>& vertices, const std::vector<unsigned>& indices, BufferUsage vertex_usage)
		{
			BufferDesc buffer_desc = {};

			buffer_desc.type = BufferType::VERTEX_BUFFER;
			buffer_desc.usage = vertex_usage;
			buffer_desc.element_count = static_cast<unsigned>(vertices.size());
			buffer_desc.stride = sizeof(vertices[0]);

			vertex_buffer_ = Buffer(buffer_desc);
			vertex_buffer_.Initialize(device_.Get(), static_cast<const void*>(vertices.data()));

			vertex_2D_ = vertices;

			buffer_desc.type = BufferType::INDEX_BUFFER;
			buffer_desc.usage = BufferUsage::STATIC_RW;
			buffer_desc.element_count = static_cast<unsigned>(indices.size());
			buffer_desc.stride = sizeof(unsigned);

			index_buffer_ = Buffer(buffer_desc);
			index_buffer_.Initialize(device_.Get(), static_cast<const void*>(indices.data()));
		}

		template<class ConstantBufferType>
		void CreateConstantBuffer(ConstantBufferType& data)
		{
			BufferDesc buffer = {};

			buffer.type = BufferType::CONSTANT_BUFFER;
			buffer.usage = BufferUsage::STATIC_RW;
			buffer.stride = sizeof(ConstantBufferType);
			buffer.element_count = 1;

			constant_buffer_ = Buffer(buffer);
			constant_buffer_.Initialize(device_.Get(), &data);
		}

		template<class ConstantBufferType>
		void UpdateConstantBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& dc, ConstantBufferType& cb, UINT register_slot, UINT buffer_num)
		{
			if (constant_buffer_.buffer_data)
			{
				dc->UpdateSubresource(constant_buffer_.buffer_data.Get(), 0, 0, &cb, 0, 0);
				dc->VSSetConstantBuffers(register_slot, buffer_num, constant_buffer_.buffer_data.GetAddressOf());
			}
		}

		void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>&);
		void CleanUp(void);

		static void SetDevice(Microsoft::WRL::ComPtr<ID3D11Device>&);

		Buffer GetVertex(void) { return vertex_buffer_; }
		Buffer GetIndex(void) { return index_buffer_; }
	};
}