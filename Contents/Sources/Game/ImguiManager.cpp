
#include"ImguiManager.h"
#include"ImGui/imgui.h"
#include"ImGui/imgui_impl_win32.h"
#include"ImGui/imgui_impl_dx11.h"
#include"..\Graphics\Graphics.h"
#include"..\Input\Input.h"

namespace Prizm
{
	class ImguiManager::Impl
	{
	public:
		Impl()
		: clear_color_(ImVec4(0.45f, 0.55f, 0.60f, 1.00f))
		, cam_pos_(ImVec4(0.f, 0.f, 0.f, 0.f)) {}

		ImVec4 clear_color_;
		ImVec4 cam_pos_;
	};

	ImguiManager::ImguiManager() : impl_(std::make_unique<Impl>()){}
	ImguiManager::~ImguiManager() = default;

	void ImguiManager::Initialize(void)
	{
		// Setup ImGui binding
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
		ImGui_ImplWin32_Init(Graphics::GetWindowHandle());
		ImGui_ImplDX11_Init(Graphics::GetDevice().Get(), Graphics::GetDeviceContext().Get());

		// Setup style
		ImGui::StyleColorsClassic();
	}

	void ImguiManager::Update(void)
	{
		{
			static float f = 0.0f;

			ImGui::PushStyleColor(ImGuiCol_TitleBgActive, impl_->clear_color_);
			ImGui::Begin("");

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
			ImGui::ColorEdit3("clear color", (float*)&impl_->clear_color_); // Edit 3 floats representing a color

			ImGui::Text("CameraPosition X : %.1f  Y : %.1f  Z : %.1f", impl_->cam_pos_.x, impl_->cam_pos_.y, impl_->cam_pos_.z);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
			ImGui::PopStyleColor();
		}
	}

	void ImguiManager::BeginFrame(void)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImguiManager::EndFrame(void)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void ImguiManager::ResizeBegin(void)
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}

	void ImguiManager::ResizeEnd(void)
	{
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	void ImguiManager::BeginGui(std::string& name)
	{
		ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoResize);
	}

	void ImguiManager::EndGui(void)
	{
		ImGui::End();
	}

	void ImguiManager::Finalize(void)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void ImguiManager::SetCameraPosition(DirectX::SimpleMath::Vector3& pos)
	{
		impl_->cam_pos_.x = pos.x;
		impl_->cam_pos_.y = pos.y;
		impl_->cam_pos_.z = pos.z;
	}
}