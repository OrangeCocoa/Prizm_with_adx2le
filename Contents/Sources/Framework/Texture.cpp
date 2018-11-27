
#include"Texture.h"
#include"Utils.h"
#include"Log.h"
#include"Resource.h"

#include"..\..\3rdParty\Includes\DirectXTex\DirectXTex.h"
#include"..\..\3rdParty\Includes\stb\stb_image.h"

/* ToDo : stb image
JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
PNG 1/2/4/8/16-bit-per-channel
TGA (not sure what subset, if a subset)
BMP non-1bpp, non-RLE
PSD (composited view only, no extra channels, 8/16 bit-per-channel)
GIF (*comp always reports as 4-channel)
HDR (radiance rgbE format)
PIC (Softimage PIC)
PNM (PPM and PGM binary only)
*/

namespace Prizm
{
	// Resource directory from Texture.cpp 
	const std::string TEXTURE_DIR = RESOURCE_DIR + "Textures/";

	class Texture::Impl
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex_2D_;
		unsigned width_, height_;

		std::string file_name_;

		Impl(void){}
	};

	Texture::Texture(void) : impl_(std::make_unique<Impl>()){}
	Texture::~Texture() = default;

	void Texture::LoadTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::string& filename)
	{
		if (filename.empty() || filename == "\"\"") return;

		impl_->file_name_ = filename;
		std::string path = TEXTURE_DIR + filename;
		std::wstring wpath(path.begin(), path.end());

		std::unique_ptr<DirectX::ScratchImage> img = std::make_unique<DirectX::ScratchImage>();

		std::string extension = path.substr(path.find_last_of("."), path.size());

		if(extension == ".tga" || extension == ".TGA")
		{
			if (succeeded(LoadFromTGAFile(wpath.c_str(), nullptr, *img)))
			{
				CreateShaderResourceView(device.Get(), img->GetImages(), img->GetImageCount(), img->GetMetadata(), &impl_->srv_);

				// get srv from img
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				impl_->srv_->GetDesc(&srvDesc);

				// read width & height
				Microsoft::WRL::ComPtr<ID3D11Resource> resource;
				impl_->srv_->GetResource(&resource);

				if (succeeded(resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(impl_->tex_2D_.GetAddressOf()))))
				{
					D3D11_TEXTURE2D_DESC desc;
					impl_->tex_2D_->GetDesc(&desc);
					impl_->width_ = desc.Width;
					impl_->height_ = desc.Height;
				}

				resource.Reset();
			}
		}
		else
		{
			if (succeeded(LoadFromWICFile(wpath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, *img)))
			{
				CreateShaderResourceView(device.Get(), img->GetImages(), img->GetImageCount(), img->GetMetadata(), &impl_->srv_);

				// get srv from img
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				impl_->srv_->GetDesc(&srvDesc);

				// read width & height
				Microsoft::WRL::ComPtr<ID3D11Resource> resource;
				impl_->srv_->GetResource(&resource);

				if (succeeded(resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(impl_->tex_2D_.GetAddressOf()))))
				{
					D3D11_TEXTURE2D_DESC desc;
					impl_->tex_2D_->GetDesc(&desc);
					impl_->width_ = desc.Width;
					impl_->height_ = desc.Height;
				}

				resource.Reset();
			}
		}
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& Texture::GetSRV(void)
	{
		return impl_->srv_;
	}

	const DirectX::SimpleMath::Vector2 Texture::GetTextureSize(void)
	{
		return DirectX::SimpleMath::Vector2(static_cast<float>(impl_->width_), static_cast<float>(impl_->height_));
	}
}