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
			DirectX::XMFLOAT4 vecWaveGeneratePosition;   // �g�̐������W
			float fAmplitude;                   // �U��
			float fWavelength;                  // �g��
			float fAngle;                       // �p�x
			float fAngularVelocity;             // �p���x
			float fWaveEdge;                    // �g�̃G�b�W
		};

		// �n���V�F�[�_�[�p�@�萔�o�b�t�@�\����
		typedef struct _CBUFFER_HS
		{
			float fTessFactor;
			float fInsideTessFactor;
			float Dummy2;
			float Dummy3;
		}CBUFFER_HS;

		// �h���C���V�F�[�_�[�p�@�萔�o�b�t�@�\����
		typedef struct _CBUFFER_DS
		{
			DirectX::XMMATRIX matCameraWorldViewProj;
			// �g�̐����ʒu
			DirectX::XMFLOAT4 vecWaveGeneratePosition[2];
			// 0:�U���A1:�g���A2:�p�x�A3�F�g�̃G�b�W
			DirectX::XMFLOAT4 vecWaveParameters[2];
		}CBUFFER_DS;

		// �s�N�Z���V�F�[�_�[�p�@�萔�o�b�t�@�\����
		typedef struct _CBUFFER_PS
		{
			// ���s�����̈ʒu�x�N�g��
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