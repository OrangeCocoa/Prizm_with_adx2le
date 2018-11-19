
#include"ImguiManager.h"
#include"ImGui/imgui.h"
#include"ImGui/imgui_impl_win32.h"
#include"ImGui/imgui_impl_dx11.h"
#include"Graphics.h"
#include"..\Input\Input.h"

namespace Prizm
{
	class ImguiManager::Impl
	{
	public:
		Impl()
		: show_demo_window_(true)
		, show_another_window_(false)
		, clear_color_(ImVec4(0.45f, 0.55f, 0.60f, 1.00f))
		, cam_pos_(ImVec4(0.f, 0.f, 0.f, 0.f)) {}

		bool show_demo_window_;
		bool show_another_window_;
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
		if (impl_->show_demo_window_)
			ImGui::ShowDemoWindow(&impl_->show_demo_window_);

		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::PushStyleColor(ImGuiCol_TitleBgActive, impl_->clear_color_);
			ImGui::Begin("Test");

			ImGui::Text("This is some useful text.");
			ImGui::Checkbox("Demo Window", &impl_->show_demo_window_);
			ImGui::Checkbox("Another Window", &impl_->show_another_window_);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
			ImGui::ColorEdit3("clear color", (float*)&impl_->clear_color_); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);
			ImGui::Text("CameraPosition X : %.1f  Y : %.1f  Z : %.1f", impl_->cam_pos_.x, impl_->cam_pos_.y, impl_->cam_pos_.z);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
			ImGui::PopStyleColor();
		}

		if (impl_->show_another_window_)
		{
			ImGui::Begin("Another Window", &impl_->show_another_window_);
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				impl_->show_another_window_ = false;
			ImGui::End();
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