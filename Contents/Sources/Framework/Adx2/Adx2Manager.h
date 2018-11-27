#pragma once

#include<string>
#include<iostream>
#include<cri_adx2le.h>

#include<DirectXTK\SimpleMath.h>

#include"..\Log.h"
#include"..\ResourcePool.h"

#pragma comment(lib, "Adx2le/cri_ware_pcx86_le_import.lib")

namespace Prizm
{
	class SoundSource
	{
	private:
		struct Source
		{
			CriAtomEx3dSourceHn source;
			CriAtomExVector source_pos;			/* sound source position */
			CriAtomExVector source_velocity;	/* sound source velocity */

			void Create(const CriAtomExPlayerHn& player)
			{
				source_pos.x = 0; source_pos.y = 0; source_pos.z = 0;
				source_velocity.x = 0; source_velocity.y = 0; source_velocity.z = 0;

				source = criAtomEx3dSource_Create(nullptr, nullptr, 0);
				criAtomExPlayer_Set3dSourceHn(player, source);

				criAtomEx3dSource_SetDopplerFactor(source, 1.f);			// doppler
				criAtomEx3dSource_SetPosition(source, &source_pos);
				criAtomEx3dSource_SetVelocity(source, &source_velocity);	// doppler
				criAtomEx3dSource_Update(source);
			}

			void Update(DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& velocity)
			{
				source_pos.x = pos.x;
				source_pos.y = pos.y;
				source_pos.z = pos.z;

				source_velocity.x += velocity.x;
				source_velocity.y += velocity.y;
				source_velocity.z += velocity.z;

				criAtomEx3dSource_SetPosition(source, &source_pos);
				criAtomEx3dSource_SetVelocity(source, &source_velocity);
				criAtomEx3dSource_Update(source);
			}

			void Destroy(void)
			{
				criAtomEx3dSource_Destroy(source);
			}
		};

		ResourcePool<Source> sound_sources_;

	public:
		// attatch 3d object
		unsigned int Create(const CriAtomExPlayerHn& player)
		{
			auto index = sound_sources_.Load(std::make_shared<Source>());
			sound_sources_.Get(index)->Create(player);
			return index;
		}

		void Update(unsigned int ss_id, DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& velocity)
		{
			sound_sources_.Get(ss_id)->Update(pos, velocity);
		}

		void Destroy(unsigned int ss_id)
		{
			sound_sources_.Get(ss_id)->Destroy();
		}

		void DestroyAll(void)
		{
			auto all_object = sound_sources_.Size();

			for (unsigned int i = 0; i < all_object; ++i)
			{
				auto ss = sound_sources_.Get(i);

				if (ss)
				{
					ss->Destroy();
					ss.reset();
				}
			}
		}
	};

	namespace Adx2Manager
	{
		enum SceneAcb
		{
			GENERAL,
			TITLE,
			GAME,
			RESULT,
			MAX
		};

		enum AtomPlayerStatus
		{
			S_ERROR,
			S_STOP,
			S_PREP,
			S_PLAYNG,
			S_PLAYEND
		};

		/* call */
		bool PositioningSoundInitialize(void);
		void PositioningSoundFinalize(void);

		bool InteractiveSoundInitialize(void);
		void InteractiveSoundFinalize(void);

		void ResetAisac(void);

		/* all */
		bool Initialize(void);
		AtomPlayerStatus Update(void);
		void Finalize(void);

		/* 3d sound */
		void PositioningSoundSourceUpdate(DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& velocity);
		void PositioningSoundListenerUpdate(DirectX::SimpleMath::Vector3& pos
			, DirectX::SimpleMath::Vector3& front, DirectX::SimpleMath::Vector3& up);

		/* interactive sound */
		void DecreaseAisac(void);
		void IncreaseAisac(void);

		// シーンごとにキューシートを切り替えするといい
		void LoadAcb(const std::string& acb_file_name, const std::string& awb_file_name, SceneAcb scene);
		void ReleaseAcb(SceneAcb scene);
		void ReleaseAcbAll(void);

		// CriAtomExPlaybackId(unsined long)型のキューIDを保存して使う
		CriAtomExPlaybackId PlayGeneralCue(CriAtomExCueId cue_id);
		CriAtomExPlaybackId PlayCurrentCue(CriAtomExCueId cue_id);
		void StopCue(CriAtomExPlaybackId id);
		void Stop(void);

		// is_paused はポーズフラグ(int型)を XOR(^=1)でスイッチングして使って
		void PauseCue(CriAtomExPlaybackId cue_id, int is_paused);
		void Pause(int is_paused);

		void UpPitchAll(void);
		void UpPitch(CriAtomExPlaybackId cue_id);
		void DownPitchAll(void);
		void DownPitch(CriAtomExPlaybackId cue_id);
		void RestorePitchAll(void);
		void RestorePitch(CriAtomExPlaybackId cue_id);

		CriAtomExPlayerHn& GetPlayer(void);
		int64_t GetTimeMS(CriAtomExPlaybackId);
	};
}