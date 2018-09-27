#pragma once

#include<d3d11.h>
#include<d3dcompiler.h>
#include<wrl/client.h>
#include"Graphics.h"
#include"..\Framework\Utils.h"
#include"..\Framework\Log.h"

namespace Prizm
{
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

	public:
		Shader(void);

		Shader(const std::string& shader_file_name);

		~Shader(void);

		bool CompileAndCreateFromFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::string& filepath, const ShaderType& type, const std::vector<D3D11_INPUT_ELEMENT_DESC>& element_desc);

		void SetVertexShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device);

		void SetPixelShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device);

		void SetGeometryShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device);

		void SetHullShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device);

		void SetDomainShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device);

		void SetComputeShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device);

		void SetInputLayout(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device);
	};
}