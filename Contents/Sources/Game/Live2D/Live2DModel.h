#pragma once

#include <Live2D\Model\CubismUserModel.hpp>
#include <ICubismModelSetting.hpp>
#include <Type/csmRectF.hpp>

#include<DirectXTK/SimpleMath.h>

namespace Prizm
{
	class Live2DModel : public Csm::CubismUserModel
	{
	private:
		Csm::ICubismModelSetting* _modelSetting; ///< ���f���Z�b�e�B���O���
		Csm::csmString _modelHomeDir; ///< ���f���Z�b�e�B���O���u���ꂽ�f�B���N�g��
		Csm::csmFloat32 _userTimeSeconds; ///< �f���^���Ԃ̐ώZ�l[�b]
		Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIds; ///<�@���f���ɐݒ肳�ꂽ�܂΂����@�\�p�p�����[�^ID
		Csm::csmVector<Csm::CubismIdHandle> _lipSyncIds; ///< ���f���ɐݒ肳�ꂽ���b�v�V���N�@�\�p�p�����[�^ID
		Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _motions; ///< �ǂݍ��܂�Ă��郂�[�V�����̃��X�g
		Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _expressions; ///< �ǂݍ��܂�Ă���\��̃��X�g
		Csm::csmVector<Csm::csmRectF> _hitArea;
		Csm::csmVector<Csm::csmRectF> _userArea;
		const Csm::CubismId* _idParamAngleX; ///< �p�����[�^ID: ParamAngleX
		const Csm::CubismId* _idParamAngleY; ///< �p�����[�^ID: ParamAngleX
		const Csm::CubismId* _idParamAngleZ; ///< �p�����[�^ID: ParamAngleX
		const Csm::CubismId* _idParamBodyAngleX; ///< �p�����[�^ID: ParamBodyAngleX
		const Csm::CubismId* _idParamEyeBallX; ///< �p�����[�^ID: ParamEyeBallX
		const Csm::CubismId* _idParamEyeBallY; ///< �p�����[�^ID: ParamEyeBallXY

		Csm::csmVector<Csm::csmUint64> _bindTextureId; ///< �e�N�X�`��ID 

		bool _deleteModel;  ///< ���̏��ŗ\��t���O Draw���Ă΂Ȃ� 

	public:
		Live2DModel(void);
		~Live2DModel(void);

		bool Initialize(void);
		void Update(void);
		void Draw(DirectX::SimpleMath::Matrix& view_projection);
		void Finalize(void);

		void LoadAsset();
		Csm::CubismMotionQueueEntryHandle StartMotion();
		Csm::CubismMotionQueueEntryHandle StartRandomMotion();
		void SetFaceMorph();
		void SetRandomFaceMorph();
	};
}