
#include"Adx2Manager.h"
#include"..\Resource.h"

namespace Prizm
{
	namespace Adx2Manager
	{
		/* MAX_VOICE is concurrent play music num */
		constexpr unsigned int MAX_VOICE = 24;
		constexpr unsigned int MAX_VIRTUAL_VOICE = 64;
		constexpr unsigned int MAX_CRIFS_LOADER = 64;

		/* Max sampling rate（include pitch change） */
		constexpr unsigned int MAX_SAMPLING_RATE = 48000 * 2;

		/* Sampling rate of HCA-MX codic */
		constexpr unsigned int SAMPLINGRATE_HCAMX = 32000;

		constexpr unsigned int X_POS_OFFSET_FOR_MAP_DRAW = 20;
		constexpr float PITCH_CHANGE_VALUE = 0.2f;
		constexpr float MAX_PITCH_VALUE = 1200.0f;

		/* Aisac control delta */
		constexpr float DELTA_AISAC_VALUE = .02f;

		const std::string path = RESOURCE_DIR + "Sounds/";
		const std::string acf_file_name = "Prizm.acf";

		/* default sound */
		CriAtomExPlayerHn       player;
		CriAtomExVoicePoolHn    std_voice_pool;		 /* voice pool(ADX/HCA codic) */
		CriAtomExVoicePoolHn    hcamx_voice_pool;       /* voice pool(HCA-MX) */
		CriAtomExAcbHn          acb_hn[SceneAcb::MAX];  /* ACB handle */
		CriAtomDbasId           dbas_id;				 /* D-BAS id*/

		/* 3d sound */
		// sample source
		CriAtomEx3dSourceHn source;
		CriAtomExVector source_pos;			/* sound source position */
		CriAtomExVector source_velocity;	/* sound source velocity */

		/* listener */
		CriAtomEx3dListenerHn listener;
		CriAtomExVector listener_pos;		/* listener position */
		CriAtomExVector listener_front;		/* listener front vector */
		CriAtomExVector listener_top;		/* listener up vector */

		/* interactive sound(cross fade) */
		CriAtomExAisacControlId  aisac_control_id;

		/* all */
		CriAtomExPlaybackId     playback_id;    /* VoiceキューのプレイバックID(再生開始時に保持) */
		SceneAcb current_scene;

		float pitch_;

		static void user_error_callback_func(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray)
		{
			const CriChar8 *errmsg = criErr_ConvertIdToMessage(errid, p1, p2);
			Log::Error(reinterpret_cast<const char*>(errmsg));
			return;
		}

		void* user_alloc_func(void *obj, CriUint32 size)
		{
			void *ptr;
			ptr = malloc(size);
			return ptr;
		}

		void user_free_func(void *obj, void *ptr)
		{
			free(ptr);
		}

		bool PositioningSoundInitialize(void)
		{
			/* 3d sound parameter */
			/*auto temp = std::make_tuple(source_pos.x, source_pos.y, source_pos.z);
			temp = std::forward_as_tuple(.0f, .0f, .0f);

			temp = std::make_tuple(source_velocity.x, source_velocity.y, source_velocity.z);
			temp = std::forward_as_tuple(.0f, .0f, .0f);

			temp = std::make_tuple(listener_pos.x, listener_pos.y, listener_pos.z);
			temp = std::forward_as_tuple(.0f, .0f, .0f);

			temp = std::make_tuple(listener_front.x, listener_front.y, listener_front.z);
			temp = std::forward_as_tuple(.0f, .0f, 1.f);

			temp = std::make_tuple(listener_top.x, listener_top.y, listener_top.z);
			temp = std::forward_as_tuple(.0f, 1.f, .0f);*/

			source_pos.x = 0; source_pos.y = 0; source_pos.z = 0;
			source_velocity.x = 0; source_velocity.y = 0; source_velocity.z = 0;
			listener_pos.x = 0; listener_pos.y = 0; listener_pos.z = 0;
			listener_front.x = 0; listener_front.y = 0; listener_front.z = 1;
			listener_top.x = 0; listener_top.y = 1; listener_top.z = 0;

			/* create object */
			source = criAtomEx3dSource_Create(nullptr, nullptr, 0);
			listener = criAtomEx3dListener_Create(nullptr, nullptr, 0);

			/* set object */
			criAtomExPlayer_Set3dSourceHn(player, source);
			criAtomExPlayer_Set3dListenerHn(player, listener);

			/* source initialize */
			criAtomEx3dSource_SetPosition(source, &source_pos);
			criAtomEx3dSource_SetVelocity(source, &source_velocity);
			criAtomEx3dSource_Update(source);

			/* listener initialize */
			criAtomEx3dListener_SetPosition(listener, &listener_pos);
			criAtomEx3dListener_SetOrientation(listener, &listener_front, &listener_top);
			criAtomEx3dListener_Update(listener);

			return true;
		}

		void PositioningSoundFinalize(void)
		{
		}

		bool InteractiveSoundInitialize(void)
		{
			/* AISAC control initialize CRI_XXXX_AISACCONTROL_ANY = 0 */
			aisac_control_id = 0;

			ResetAisac();

			return true;
		}

		void InteractiveSoundFinalize(void)
		{
			ResetAisac();
		}

		void ResetAisac(void)
		{
			criAtomExPlayer_SetAisacById(player, aisac_control_id, .0f);
		}

		bool Initialize(void)
		{
			CoInitializeEx(NULL, COINIT_MULTITHREADED);

			playback_id = 0;
			pitch_ = 0;
			current_scene = SceneAcb::GENERAL;

			criErr_SetCallback(user_error_callback_func);

			/* メモリアロケータの登録 */
			criAtomEx_SetUserAllocator(user_alloc_func, user_free_func, nullptr);

			/* ライブラリの初期化（最大ボイス数変更） */
			CriAtomExConfig_WASAPI init_config;
			CriFsConfig fs_config;
			criAtomEx_SetDefaultConfig_WASAPI(&init_config);
			criFs_SetDefaultConfig(&fs_config);
			init_config.atom_ex.max_virtual_voices = MAX_VIRTUAL_VOICE;
			init_config.hca_mx.output_sampling_rate = SAMPLINGRATE_HCAMX;
			fs_config.num_loaders = MAX_CRIFS_LOADER;
			init_config.atom_ex.fs_config = &fs_config;
			criAtomEx_Initialize_WASAPI(&init_config, nullptr, 0);

			/* D-Basの作成（最大ストリーム数はここで決まります） */
			dbas_id = criAtomDbas_Create(nullptr, nullptr, 0);

			std::string acf_path(path + acf_file_name);

			/* ACFファイルの読み込みと登録 */
			criAtomEx_RegisterAcfFile(nullptr, acf_path.c_str(), nullptr, 0);

			/* DSP attatch */
			criAtomEx_AttachDspBusSetting("DspBusSetting_0", nullptr, 0);

			/* standard voice pool create（最大ボイス数変更／最大ピッチ変更／ストリーム再生対応） */
			CriAtomExStandardVoicePoolConfig std_vpool_config;
			criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&std_vpool_config);
			std_vpool_config.num_voices = MAX_VOICE;
			std_vpool_config.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
			std_vpool_config.player_config.streaming_flag = CRI_TRUE;
			std_voice_pool = criAtomExVoicePool_AllocateStandardVoicePool(&std_vpool_config, nullptr, 0);

			/* HCA-MX：voice pool create */
			CriAtomExHcaMxVoicePoolConfig hcamx_vpool_config;
			criAtomExVoicePool_SetDefaultConfigForHcaMxVoicePool(&hcamx_vpool_config);
			hcamx_vpool_config.num_voices = MAX_VOICE;
			hcamx_vpool_config.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
			hcamx_vpool_config.player_config.streaming_flag = CRI_TRUE;
			hcamx_voice_pool = criAtomExVoicePool_AllocateHcaMxVoicePool(&hcamx_vpool_config, nullptr, 0);

			/* create player */
			player = criAtomExPlayer_Create(nullptr, nullptr, 0);

			PositioningSoundInitialize();
			InteractiveSoundInitialize();

			// 最大ピッチを設定していると、範囲内のピッチ変更ならピッチ変更時のバッファリングなしに変更してくれるっぽい
			criAtomExPlayer_SetMaxPitch(player, MAX_PITCH_VALUE);

			return true;
		}

		AtomPlayerStatus Update(void)
		{
			criAtomEx_ExecuteMain();

#if defined(USE_INGAME_PREVIEW)
			/* バス解析情報の取得 */
			CriAtomExAsrBusAnalyzerInfo analyze_info;
			CriSint32 bus_no;
			for (bus_no = 0; bus_no < 8; bus_no++) {
				criAtomExAsr_GetBusAnalyzerInfo(bus_no, &analyze_info);
			}
#endif

			/* get current status */
			CriAtomExPlayerStatus playerstatus;
			playerstatus = criAtomExPlayer_GetStatus(player);

			switch (playerstatus)
			{
			case CRIATOMEXPLAYER_STATUS_ERROR:
				return S_ERROR;

			case CRIATOMEXPLAYER_STATUS_PREP:
				return S_PREP;

			case CRIATOMEXPLAYER_STATUS_PLAYING:
				return S_PLAYNG;

			case CRIATOMEXPLAYER_STATUS_PLAYEND:
				return S_PLAYEND;
			}

			return S_STOP;
		}

		void Finalize(void)
		{
			criAtomEx_DetachDspBusSetting();

			criAtomExPlayer_Destroy(player);

			criAtomExVoicePool_Free(hcamx_voice_pool);
			criAtomExVoicePool_Free(std_voice_pool);

			criAtomExAcb_ReleaseAll();

			criAtomEx_UnregisterAcf();

#if defined(USE_INGAME_PREVIEW)
			/* インゲームプレビュー関連機能の終了処理 */
			CriSint32 bus_no;
			for (bus_no = 0; bus_no < 8; bus_no++)
			{
				criAtomExAsr_DetachBusAnalyzer(bus_no);
			}
			criAtomExMonitor_Finalize();
#endif

			criAtomDbas_Destroy(dbas_id);

			criAtomEx_Finalize_WASAPI();

			CoUninitialize();
		}

		void PositioningSoundSourceUpdate(DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& velocity)
		{
			source_pos.x += pos.x;
			source_pos.y += pos.y;
			source_pos.z += pos.z;

			source_velocity.x += velocity.x;
			source_velocity.y += velocity.y;
			source_velocity.z += velocity.z;

			criAtomEx3dSource_SetPosition(source, &source_pos);
			criAtomEx3dSource_SetVelocity(source, &source_velocity);
			criAtomEx3dSource_Update(source);
		}

		void PositioningSoundListenerUpdate(DirectX::SimpleMath::Vector3& pos
			, DirectX::SimpleMath::Vector3& front, DirectX::SimpleMath::Vector3& up)
		{
			listener_pos.x = pos.x;
			listener_pos.y = pos.y;
			listener_pos.z = pos.z;

			listener_front.x = front.x;
			listener_front.y = front.y;
			listener_front.z = front.z;

			listener_top.x = up.x;
			listener_top.y = up.y;
			listener_top.z = up.z;

			criAtomEx3dListener_SetPosition(listener, &listener_pos);
			criAtomEx3dListener_SetOrientation(listener, &listener_front, &listener_top);
			criAtomEx3dListener_Update(listener);
		}

		void DecreaseAisac(void)
		{
			CriFloat32 aisac_val = criAtomExPlayer_GetAisacById(player, aisac_control_id) - DELTA_AISAC_VALUE;
			criAtomExPlayer_SetAisacById(player, aisac_control_id, aisac_val);
			criAtomExPlayer_UpdateAll(player);
		}

		void IncreaseAisac(void)
		{
			CriFloat32 aisac_val = criAtomExPlayer_GetAisacById(player, aisac_control_id) + DELTA_AISAC_VALUE;
			criAtomExPlayer_SetAisacById(player, aisac_control_id, aisac_val);
			criAtomExPlayer_UpdateAll(player);
		}

		void LoadAcb(const std::string& acb_file_name, const std::string& awb_file_name, SceneAcb scene)
		{
			std::string acb_path = path + acb_file_name;
			std::string awb_path = path + awb_file_name;

			acb_hn[scene] = criAtomExAcb_LoadAcbFile(nullptr, acb_path.c_str(), nullptr, awb_path.c_str(), nullptr, 0);

			current_scene = scene;
		}

		void ReleaseAcb(SceneAcb scene)
		{
			criAtomExAcb_Release(acb_hn[scene]);
			current_scene = SceneAcb::GENERAL;
		}

		void ReleaseAcbAll(void)
		{
			criAtomExAcb_ReleaseAll();
		}

		CriAtomExPlaybackId PlayGeneralCue(CriAtomExCueId start_cue_id)
		{
			criAtomExPlayer_SetCueId(player, acb_hn[SceneAcb::GENERAL], start_cue_id);

			return playback_id = criAtomExPlayer_Start(player);
		}

		CriAtomExPlaybackId PlayCurrentCue(CriAtomExCueId start_cue_id)
		{
			criAtomExPlayer_SetCueId(player, acb_hn[current_scene], start_cue_id);

			return playback_id = criAtomExPlayer_Start(player);
		}

		void StopCue(CriAtomExPlaybackId cue_id)
		{
			criAtomExPlayback_Stop(cue_id);

			/* ToDo */
			ResetAisac();
		}

		void Stop(void)
		{
			criAtomExPlayer_Stop(player);
			ResetAisac();
		}

		void PauseCue(CriAtomExPlaybackId cue_id, int is_paused)
		{
			criAtomExPlayback_Pause(cue_id, is_paused);
		}

		void Pause(int is_paused)
		{
			criAtomExPlayer_Pause(player, is_paused);
		}

		void UpPitchAll(void)
		{
			if (pitch_ < MAX_PITCH_VALUE)
				pitch_ += PITCH_CHANGE_VALUE;

			criAtomExPlayer_SetPitch(player, pitch_);
			criAtomExPlayer_UpdateAll(player);
		}
		
		void UpPitch(CriAtomExPlaybackId cue_id)
		{
			if (pitch_ < MAX_PITCH_VALUE)
				pitch_ += PITCH_CHANGE_VALUE;

			criAtomExPlayer_SetPitch(player, pitch_);
			criAtomExPlayer_Update(player, cue_id);
		}

		void DownPitchAll(void)
		{
			if (pitch_ > -MAX_PITCH_VALUE)
				pitch_ -= PITCH_CHANGE_VALUE;

			criAtomExPlayer_SetPitch(player, pitch_);
			criAtomExPlayer_UpdateAll(player);
		}
		
		void DownPitch(CriAtomExPlaybackId cue_id)
		{
			if (pitch_ > -MAX_PITCH_VALUE)
				pitch_ -= PITCH_CHANGE_VALUE;

			criAtomExPlayer_SetPitch(player, pitch_);
			criAtomExPlayer_Update(player, cue_id);
		}

		void RestorePitchAll(void)
		{
			criAtomExPlayer_SetPitch(player, 0);
			criAtomExPlayer_UpdateAll(player);
		}
		
		void RestorePitch(CriAtomExPlaybackId cue_id)
		{
			criAtomExPlayer_SetPitch(player, 0);
			criAtomExPlayer_Update(player, cue_id);
		}

		CriAtomExPlayerHn& GetPlayer(void)
		{
			return player;
		}

		int64_t GetTimeMS(CriAtomExPlaybackId playback_id)
		{
			return criAtomExPlayback_GetTime(playback_id);
		}
	}
}