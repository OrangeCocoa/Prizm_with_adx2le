#pragma once
#include<vector>

#include <Rendering/D3D11/CubismNativeInclude_D3D11.hpp>
#include <CubismFramework.hpp>

#pragma comment(lib, "Live2D/Framework.lib")
#pragma comment(lib, "Live2D/Live2DCubismCore.lib")

#include"Live2DModel.h"

namespace Prizm
{
	class Live2DManager
	{
	private:
		std::vector<Live2DModel> models_;

	public:
		bool Initialize(void);
		void Finalize(void);
	};
}