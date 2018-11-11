#pragma once

#include<vector>
#include<wrl\client.h>
#include<DirectXTK/SimpleMath.h>

#include"Buffer.h"
#include"Texture.h"
#include"ConstantBuffer.h"
#include"..\Window\Window.h"

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
		TopologyType topology_;
		std::vector<VertexBuffer2D> vertex_2D_;

	public:
		template<class VertexBufferType>
		Geometry(const std::vector<VertexBufferType>& vertices, const std::vector<unsigned>& indices, TopologyType topology)
		{//3D
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

			topology_ = topology;
		}
		
		Geometry(const std::vector<VertexBuffer2D>& vertices, const std::vector<unsigned>& indices, BufferUsage vertex_usage)
		{//2D
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

			topology_ = TopologyType::TRIANGLE_LIST;
		}

		void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>&);
		void CleanUp(void);

		static void SetDevice(Microsoft::WRL::ComPtr<ID3D11Device>&);

		Buffer GetVertex(void) { return vertex_buffer_; }
		Buffer GetIndex(void) { return index_buffer_; }

		void MovePosition2DScreenToRatio(float x, float y)
		{
			for (auto vtx : vertex_2D_)
			{
				vtx.position.x += x / window_width<float>;
				vtx.position.y += y / window_height<float>;
			}
		}
	};
}