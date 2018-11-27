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
		Csm::ICubismModelSetting* _modelSetting; ///< モデルセッティング情報
		Csm::csmString _modelHomeDir; ///< モデルセッティングが置かれたディレクトリ
		Csm::csmFloat32 _userTimeSeconds; ///< デルタ時間の積算値[秒]
		Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIds; ///<　モデルに設定されたまばたき機能用パラメータID
		Csm::csmVector<Csm::CubismIdHandle> _lipSyncIds; ///< モデルに設定されたリップシンク機能用パラメータID
		Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _motions; ///< 読み込まれているモーションのリスト
		Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _expressions; ///< 読み込まれている表情のリスト
		Csm::csmVector<Csm::csmRectF> _hitArea;
		Csm::csmVector<Csm::csmRectF> _userArea;
		const Csm::CubismId* _idParamAngleX; ///< パラメータID: ParamAngleX
		const Csm::CubismId* _idParamAngleY; ///< パラメータID: ParamAngleX
		const Csm::CubismId* _idParamAngleZ; ///< パラメータID: ParamAngleX
		const Csm::CubismId* _idParamBodyAngleX; ///< パラメータID: ParamBodyAngleX
		const Csm::CubismId* _idParamEyeBallX; ///< パラメータID: ParamEyeBallX
		const Csm::CubismId* _idParamEyeBallY; ///< パラメータID: ParamEyeBallXY

		Csm::csmVector<Csm::csmUint64> _bindTextureId; ///< テクスチャID 

		bool _deleteModel;  ///< 実体消滅予定フラグ Drawを呼ばない 

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