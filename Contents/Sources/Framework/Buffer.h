#pragma once

#include<wrl/client.h>
#include<d3d11_4.h>

#include<DirectXTK/SimpleMath.h>

namespace Prizm
{
	enum BufferUsage
	{
		// https://msdn.microsoft.com/ja-jp/library/ee422122(v=vs.85).aspx
		STATIC_R = D3D11_USAGE_IMMUTABLE,	// GPU_R
		STATIC_RW = D3D11_USAGE_DEFAULT,	// GPU_RW
		DYNAMIC = D3D11_USAGE_DYNAMIC,		// GPU_R / CPU_W
		STAGING = D3D11_USAGE_STAGING,		// GPU_R / CPU_R

		BUFFER_USAGE_MAX
	};

	enum BufferType
	{
		VERTEX_BUFFER = D3D11_BIND_VERTEX_BUFFER,
		INDEX_BUFFER = D3D11_BIND_INDEX_BUFFER,
		CONSTANT_BUFFER = D3D11_BIND_CONSTANT_BUFFER,

		BUFFER_TYPE_UNKNOWN,
		BUFFER_TYPE_MAX
	};

	struct BufferDesc
	{
		BufferUsage usage;
		BufferType type;
		unsigned element_count;
		unsigned stride;	// sizeof(type)

		BufferDesc() : usage(STATIC_R), type(BUFFER_TYPE_UNKNOWN), element_count(0), stride(0){}
	};

	struct Buffer
	{
		BufferDesc desc;
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_data;
		//bool dirty;
		//void* cpu_data_cache; // dynamic texture
		bool initialized;
		std::allocator<char> allocator;

		Buffer() : buffer_data(nullptr), initialized(false){}
		Buffer(const BufferDesc&);

		void Initialize(ID3D11Device* = nullptr, const void* = nullptr);

		// dynamic only
		void Update(Microsoft::WRL::ComPtr<ID3D11DeviceContext>&, const void*);

		void CleanUp(void);
	};

	struct VertexBuffer2D
	{// input layout
		DirectX::SimpleMath::Vector2 position;
		DirectX::SimpleMath::Vector4 color;
		DirectX::SimpleMath::Vector2 uv;
	};
	
	struct VertexBuffer3D
	{// input layout
		DirectX::SimpleMath::Vector4 position;
		DirectX::SimpleMath::Vector4 color;
		DirectX::SimpleMath::Vector2 uv;
		DirectX::SimpleMath::Vector3 normal;
		DirectX::SimpleMath::Vector3 tangent;
	};
	
}