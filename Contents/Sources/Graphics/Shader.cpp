
#include"Shader.h"

namespace Prizm
{
	// Resource directory from Shader.cpp
	constexpr const char* SHADER_RESOURCE_DIR = "../../Resource/Shaders/";
	constexpr const char* SHADER_COMPILER_VERSIONS[] = { "vs_5_0", "gs_5_0", "hs_5_0", "ds_5_0", "cs_5_0", "ps_5_0" };
	constexpr const char* SHADER_ENTRY_POINTS[] = { "VSMain", "GSMain", "DSMain", "HSMain", "CSMain", "PSMain" };
#ifdef _DEBUG
	constexpr UINT SHADER_COMPILE_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
	constexpr UINT SHADER_COMPILE_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

	class Shader::Impl
	{
	public:
		std::string name_;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader > ps_;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs_;
		Microsoft::WRL::ComPtr<ID3D11HullShader> hs_;
		Microsoft::WRL::ComPtr<ID3D11DomainShader> ds_;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> cs_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layput_;

		std::vector<ShaderTexture> textures_;

		Impl()
			: name_("")
			, vs_(nullptr)
			, ps_(nullptr)
			, gs_(nullptr)
			, hs_(nullptr)
			, ds_(nullptr)
			, cs_(nullptr)
			, input_layput_(nullptr){}
		
		Impl(const std::string& shader_file_name)
			: name_(shader_file_name)
			, vs_(nullptr)
			, ps_(nullptr)
			, gs_(nullptr)
			, hs_(nullptr)
			, ds_(nullptr)
			, cs_(nullptr)
			, input_layput_(nullptr){}

		bool CreateShader(Microsoft::WRL::ComPtr<ID3D11Device>& device, ShaderType type, void* buffer, const size_t shader_binary_size)
		{
			switch (type)
			{
			case ShaderType::VS:
				if (failed(device->CreateVertexShader(buffer, shader_binary_size, nullptr, vs_.GetAddressOf())))
				{
					Log::Error("Error creating vertex shader program.");
					return false;
				}
				break;
			case ShaderType::PS:
				if (failed(device->CreatePixelShader(buffer, shader_binary_size, nullptr, ps_.GetAddressOf())))
				{
					Log::Error("Error creating pixel shader program.");
					return false;
				}
				break;
			case ShaderType::GS:
				if (failed(device->CreateGeometryShader(buffer, shader_binary_size, nullptr, gs_.GetAddressOf())))
				{
					Log::Error("Error creating pixel geometry program.");
					return false;
				}
				break;
			case ShaderType::HS:
				if (failed(device->CreateHullShader(buffer, shader_binary_size, nullptr, hs_.GetAddressOf())))
				{
					Log::Error("Error creating hull shader program.");
					return false;
				}
				break;
			case ShaderType::DS:
				if (failed(device->CreateDomainShader(buffer, shader_binary_size, nullptr, ds_.GetAddressOf())))
				{
					Log::Error("Error creating domain shader program.");
					return false;
				}
				break;
			case ShaderType::CS:
				if (failed(device->CreateComputeShader(buffer, shader_binary_size, nullptr, cs_.GetAddressOf())))
				{
					Log::Error("Error creating conpute shader program.");
					return false;
				}
				break;
			}
			return true;
		}

		bool CreateInputLayout(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::vector<D3D11_INPUT_ELEMENT_DESC>& element_desc, Microsoft::WRL::ComPtr<ID3DBlob>& blob)
		{
			if (failed(device->CreateInputLayout(&element_desc[0], element_desc.size(), blob->GetBufferPointer(), blob->GetBufferSize(), input_layput_.GetAddressOf())))
			{
				Log::Error("Error creating input layout.");
				return false;
			}
			return true;
		}
	};

	Shader::Shader()
		: impl_(std::make_unique<Impl>()) {}

	Shader::Shader(const std::string& shader_file_name)
		: impl_(std::make_unique<Impl>(shader_file_name)) {}

	Shader::~Shader() = default;

	bool Shader::CompileAndCreateFromFile(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::string& filepath, const ShaderType& type, const std::vector<D3D11_INPUT_ELEMENT_DESC>& element_desc)
	{
		impl_->name_ = filepath;

		std::string dir_path = SHADER_RESOURCE_DIR + filepath;
		const std::wstring path = StrUtils::AsciiToUnicode(dir_path.c_str());

		Microsoft::WRL::ComPtr<ID3DBlob> blob;
		Microsoft::WRL::ComPtr<ID3DBlob> error_blob;

		if (failed(D3DCompileFromFile(
			path.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			SHADER_ENTRY_POINTS[type],
			SHADER_COMPILER_VERSIONS[type],
			SHADER_COMPILE_FLAGS,
			0,
			blob.GetAddressOf(),
			error_blob.GetAddressOf())))
		{
			Log::Error(static_cast<char*>(error_blob->GetBufferPointer()));
			return false;
		}

		if (blob)
		{
			if (failed(impl_->CreateShader(device, type, blob->GetBufferPointer(), blob->GetBufferSize())))
			{
				Log::Error("Do not create shader.");
				return false;
			}
		}
		else
		{
			Log::Error("ID3DBlob is nullptr.");
			Log::Error(static_cast<char*>(error_blob->GetBufferPointer()));
			return false;
		}

		if (type == ShaderType::VS)
		{
			if (failed(impl_->CreateInputLayout(device, element_desc, blob)))
			{
				Log::Error("Do not create shader.");
			}
		}
		return true;
	}

	void Shader::SetVertexShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device)
	{
		device->VSSetShader(impl_->vs_.Get(), nullptr, 0);
	}

	void Shader::SetPixelShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device)
	{
		device->PSSetShader(impl_->ps_.Get(), nullptr, 0);
	}

	void Shader::SetGeometryShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device)
	{
		device->GSSetShader(impl_->gs_.Get(), nullptr, 0);
	}

	void Shader::SetHullShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device)
	{
		device->HSSetShader(impl_->hs_.Get(), nullptr, 0);
	}

	void Shader::SetDomainShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device)
	{
		device->DSSetShader(impl_->ds_.Get(), nullptr, 0);
	}

	void Shader::SetComputeShader(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device)
	{
		device->CSSetShader(impl_->cs_.Get(), nullptr, 0);
	}

	void Shader::SetInputLayout(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device)
	{
		device->IASetInputLayout(impl_->input_layput_.Get());
	}
}