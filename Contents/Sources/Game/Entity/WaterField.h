#pragma once

#include<memory>
#include<SimpleMath.h>

namespace PrizmEngine
{
	class Graphics;
	class Camera;

	class Field
	{
	private:
		class Impl;
		std::unique_ptr<Impl> impl_;

		const std::unique_ptr<Graphics>& graphics_;

		struct WAVE
		{
			DirectX::XMFLOAT4 vecWaveGeneratePosition;   // 波の生成座標
			float fAmplitude;                   // 振幅
			float fWavelength;                  // 波長
			float fAngle;                       // 角度
			float fAngularVelocity;             // 角速度
			float fWaveEdge;                    // 波のエッジ
		};

		// ハルシェーダー用　定数バッファ構造体
		typedef struct _CBUFFER_HS
		{
			float fTessFactor;
			float fInsideTessFactor;
			float Dummy2;
			float Dummy3;
		}CBUFFER_HS;

		// ドメインシェーダー用　定数バッファ構造体
		typedef struct _CBUFFER_DS
		{
			DirectX::XMMATRIX matCameraWorldViewProj;
			// 波の生成位置
			DirectX::XMFLOAT4 vecWaveGeneratePosition[2];
			// 0:振幅、1:波長、2:角度、3：波のエッジ
			DirectX::XMFLOAT4 vecWaveParameters[2];
		}CBUFFER_DS;

		// ピクセルシェーダー用　定数バッファ構造体
		typedef struct _CBUFFER_PS
		{
			// 平行光源の位置ベクトル
			DirectX::XMFLOAT4 vecLightPos;
		}CBUFFER_PS;

	public:
		Field(std::unique_ptr<Graphics>& graphics);
		~Field(void);

		bool Init(void);
		void Run(void);
		void Draw(std::unique_ptr<Camera>&);
		bool LoadShader(void);
		//void SetShader(std::unique_ptr<Shader>&);
	};
}