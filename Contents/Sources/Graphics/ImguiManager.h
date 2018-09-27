#pragma once

#include<memory>
#include<string>
#include<DirectXTK/SimpleMath.h>

namespace Prizm
{
	class Graphics;

	class ImguiManager
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		ImguiManager();
		~ImguiManager();

		void Initialize(const std::unique_ptr<Graphics>&);
		void Update(void);
		void BeginFrame(void);
		void EndFrame(void);
		void ResizeBegin(void);
		void ResizeEnd(void);
		void BeginGui(std::string&);
		void EndGui(void);
		void Exit(void);

		void SetCameraPosition(DirectX::SimpleMath::Vector3&);
	};
}