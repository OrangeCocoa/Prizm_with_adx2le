
#include<memory>

#include"TileRenderer.h"
#include"..\Framework\Shader.h"
#include"..\Framework\Buffer.h"

namespace Prizm
{
	class TileRenderer::Impl
	{
	public:
		std::unique_ptr<Shader> tile_compute_;

		Impl() 
			: tile_compute_(std::make_unique<Shader>()){}
	};

	TileRenderer::TileRenderer() : impl_(std::make_unique<Impl>()){}

	TileRenderer::~TileRenderer() = default;

	void TileRenderer::Initialize(Device device)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc = {};
		impl_->tile_compute_->CompileAndCreateFromFile(device, "TileForward_cs.hlsl", ShaderType::CS, desc);
	}

	void TileRenderer::DispatchForward(DeviceContext device_context)
	{
		impl_->tile_compute_->SetShader(device_context, ShaderType::CS);

		BufferDesc desc = {};
		desc.type = BufferType::CONSTANT_BUFFER;

		impl_->tile_compute_->Dispatch(device_context, 0, 0, 0);
	}
}