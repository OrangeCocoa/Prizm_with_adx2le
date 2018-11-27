
#include"Buffer.h"
#include"Utils.h"
#include"Log.h"

namespace Prizm
{
	Buffer::Buffer(const BufferDesc& data) : desc(data), buffer_data(nullptr){}

	void Buffer::Initialize(ID3D11Device* device, const void* data)
	{
		D3D11_BUFFER_DESC buffer_desc;
		buffer_desc.Usage = static_cast<D3D11_USAGE>(desc.usage);
		buffer_desc.BindFlags = static_cast<D3D11_BIND_FLAG>(desc.type);
		buffer_desc.ByteWidth = desc.stride * desc.element_count;
		buffer_desc.CPUAccessFlags = desc.usage == BufferUsage::DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subresource_data = {};
		D3D11_SUBRESOURCE_DATA* srd_ptr = nullptr;

		if (data)
		{
			subresource_data.pSysMem = data;
			subresource_data.SysMemPitch = 0;
			subresource_data.SysMemSlicePitch = 0;
			srd_ptr = &subresource_data;
		}

		if (failed(device->CreateBuffer(&buffer_desc, srd_ptr, &buffer_data)))
		{
			Log::Error("Failed to create buffer. (Buffer.cpp)");
		}
	}

	void Buffer::Update(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& dc, const void* data)
	{// Dynamically change buffer data.

		D3D11_MAPPED_SUBRESOURCE mapped_resource = {};
		constexpr UINT subresource = 0;
		constexpr UINT map_flags = 0;
		const UINT size = desc.stride * desc.element_count;

		dc->Map(buffer_data.Get(), subresource, D3D11_MAP_WRITE_DISCARD, map_flags, &mapped_resource);
		memcpy(mapped_resource.pData, data, size);
		dc->Unmap(buffer_data.Get(), subresource);
	}

	void Buffer::CleanUp(void)
	{
		if (buffer_data)
		{
			buffer_data.Reset();
		}
	}
}