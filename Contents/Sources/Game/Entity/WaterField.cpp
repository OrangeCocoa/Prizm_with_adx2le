
#include"WaterField.h"
#include"..\..\Renderer\Mesh.h"
#include"..\..\Renderer\Shader.h"
#include"..\..\Renderer\GeometryGenerator.h"

namespace PrizmEngine
{
	class Field::Impl
	{
	public:
		std::unique_ptr<Mesh> mesh_;
		std::unique_ptr<Shader> shader_;

		WAVE waves_[2];

		Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers_[3];

		int pass_;

		DirectX::XMMATRIX view_, proj_;
		DirectX::XMFLOAT4 m_VecLightPos;

		D3D11_FILL_MODE fill_mode_;

		Impl() : shader_(std::make_unique<Shader>()) {}

		void Invalidate();
		void Create(WAVE pWave[2]);
		void Begin(DirectX::XMFLOAT4* pVecLightPos, DirectX::XMMATRIX* pMatCameraView, DirectX::XMMATRIX* pMatCameraProj, D3D11_FILL_MODE FillMode = D3D11_FILL_SOLID);
		void BeginPass(UINT Pass);
		void SetConstantBuffers(DirectX::XMMATRIX* pMatWorld, float fTessFactor, float fInsideTessFactor);
		void EndPass();
		void End();
	};

	Field::Field(std::unique_ptr<Graphics>& graphics) : graphics_(graphics), impl_(std::make_unique<Impl>()){}
	Field::~Field(void) = default;

	bool Field::Init(void)
	{
		impl_->fill_mode_ = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		impl_->pass_ = -2;
		impl_->mesh_ = std::make_unique<Mesh>(PrimitiveGeometry::Quad3D(100.0f));

		WAVE waves_[2];

		waves_[0].vecWaveGeneratePosition = DirectX::XMFLOAT4(1200.0f, 0.0f, 200.0f, 0);
		waves_[0].fAmplitude = 10.0f;              // U•
		waves_[0].fWavelength = 20.0f;             // ”g’·
		waves_[0].fAngularVelocity = -0.001f;      // Šp‘¬“x
		waves_[0].fWaveEdge = 1.0f;                // ”g‚ÌƒGƒbƒW
		waves_[0].fAngle = 0.0f;                   // Šp“x

		waves_[1].vecWaveGeneratePosition = DirectX::XMFLOAT4(-1200.0f, 0.0f, -400.0f, 0);
		waves_[1].fAmplitude = 15.0f;
		waves_[1].fWavelength = 50.0f;
		waves_[1].fAngularVelocity = -0.002f;
		waves_[1].fWaveEdge = 2.5f;
		waves_[1].fAngle = 0.0f;

		impl_->Create(waves_);

		return true;
	}
	void Field::Run(void)
	{
	}
	void Field::Draw(std::unique_ptr<Camera>&)
	{
	}
	bool Field::LoadShader(void)
	{
		return false;
	}
}