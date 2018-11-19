#pragma once

#include<d3d11.h>
#include<d3dcompiler.h>
#include<wrl/client.h>

#include<vector>

#include"Graphics.h"
#include"Buffer.h"

namespace Prizm
{
	enum ShaderFile
	{
		OBJECT_2D_SHADER,
		OBJECT_3D_SHADER,
		SHADOW_MAP_SHADER,
		HDAO_SHADER,
		TILEBASE_COMPUTE_SHADER,

		SHADER_MAX
	};

	enum ShaderType : unsigned
	{
		VS = 0,
		GS,
		DS,
		HS,
		CS,
		PS,

		SHADER_TYPE_MAX
	};

	enum LayoutFormat
	{// DXGI_FORMAT
		FLOAT32_2 = DXGI_FORMAT_R32G32_FLOAT,
		FLOAT32_3 = DXGI_FORMAT_R32G32B32_FLOAT,
		FLOAT32_4 = DXGI_FORMAT_R32G32B32A32_FLOAT,

		LAYOUT_FORMAT_MAX
	};

	struct ConstantBuffer
	{	
		ShaderType type;
		unsigned	buffer_slot;
		Microsoft::WRL::ComPtr<ID3D11Buffer> data;
		bool dirty;
	};

	struct ShaderTexture
	{
		std::string name;
		unsigned buffer_slot;
		ShaderType type;
	};

	struct ShaderSampler
	{
		std::string name;
		unsigned buffer_slot;
		ShaderType type;
	};

	struct InputLayout
	{
		std::string		name;
		LayoutFormat	format;
	};

	class Shader
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

		std::vector<Buffer> constant_buffer_;

	public:
		Shader(void);

		Shader(const std::string& shader_file_name);

		~Shader(void);

		bool CompileAndCreateFromFile(Microsoft::WRL::ComPtr<ID3D11Device>& device,
			const ShaderType& type, const std::vector<D3D11_INPUT_ELEMENT_DESC>& element_desc);
		
		bool CompileAndCreateFromFile(Microsoft::WRL::ComPtr<ID3D11Device>& device,
			const std::string& filepath, const ShaderType& type, const std::vector<D3D11_INPUT_ELEMENT_DESC>& element_desc);

		void SetShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device, const ShaderType& type);

		void SetInputLayout(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device);

		// the order call register slot
		template<class ConstantBufferType>
		void CreateConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device, ConstantBufferType& data)
		{
			BufferDesc buffer = {};

			buffer.type = BufferType::CONSTANT_BUFFER;
			buffer.usage = BufferUsage::STATIC_RW;
			buffer.stride = sizeof(ConstantBufferType);
			buffer.element_count = 1;

			constant_buffer_.emplace_back(Buffer(buffer));
			constant_buffer_.back().Initialize(device.Get(), &data);
		}

		template<class ConstantBufferType>
		void UpdateConstantBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& dc,
			ConstantBufferType& cb, ShaderType type, 
			unsigned int register_slot, unsigned int buffer_num)
		{
			Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer = constant_buffer_[register_slot].buffer_data;

			if (constant_buffer)
			{
				dc->UpdateSubresource(constant_buffer.Get(), 0, 0, &cb, 0, 0);

				switch (type)
				{
				case ShaderType::VS:
					dc->VSSetConstantBuffers(register_slot, buffer_num, constant_buffer.GetAddressOf());
					break;
				case ShaderType::PS:
					dc->PSSetConstantBuffers(register_slot, buffer_num, constant_buffer.GetAddressOf());
					break;
				case ShaderType::GS:
					dc->GSSetConstantBuffers(register_slot, buffer_num, constant_buffer.GetAddressOf());
					break;
				case ShaderType::HS:
					dc->HSSetConstantBuffers(register_slot, buffer_num, constant_buffer.GetAddressOf());
					break;
				case ShaderType::DS:
					dc->DSSetConstantBuffers(register_slot, buffer_num, constant_buffer.GetAddressOf());
					break;
				case ShaderType::CS:
					dc->CSSetConstantBuffers(register_slot, buffer_num, constant_buffer.GetAddressOf());
					break;
				}
			}
		}

		void SetShaderResources(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device_context,
			const ShaderType& type, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv,
			unsigned int register_slot, unsigned int buffer_num);

		void SetUnorderedAccessView(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device_context,
			Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& uavs,
			const unsigned int* uav_inits,
			unsigned int register_slot, unsigned int buffer_num);

		void Dispatch(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device_context,
			unsigned int thread_group_x, unsigned int thread_group_y, unsigned int thread_group_z);
	};
}