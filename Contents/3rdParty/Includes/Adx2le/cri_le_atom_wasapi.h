/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2013 CRI Middleware Co., Ltd.
 *
 * Library  : CRI Atom
 * Module   : Library User's Header for WASAPI
 * File     : cri_le_atom_wasapi.h
 *
 ****************************************************************************/
/*!
 *	\file		cri_le_atom_wasapi.h
 */

/* 多重定義防止					*/
/* Prevention of redefinition	*/
#ifndef CRI_INCL_CRI_ATOM_WASAPI_H
#define CRI_INCL_CRI_ATOM_WASAPI_H

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
#include <windows.h>
#include <audioclient.h>
#include <ks.h>
#include <ksmedia.h>
#include <audiosessiontypes.h>
#include <cri_le_xpt.h>
#include <cri_le_atom.h>
#include <cri_le_atom_ex.h>
#include <cri_le_atom_asr.h>

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/
/*==========================================================================
 *      CRI Atom API
 *=========================================================================*/

/*==========================================================================
 *      CRI AtomEx API
 *=========================================================================*/
/*JP
 * \brief ライブラリ初期化用コンフィグ構造体にデフォルト値をセット
 * \ingroup ATOMLIB_WASAPI
 * \param[out]	p_config	初期化用コンフィグ構造体へのポインタ
 * \par 説明:
 * ::criAtomEx_Initialize_WASAPI 関数に設定するコンフィグ構造体
 * （ ::CriAtomExConfig_WASAPI ）に、デフォルトの値をセットします。<br>
 * \sa CriAtomExConfig_WASAPI
 */
#define criAtomEx_SetDefaultConfig_WASAPI(p_config)			\
{															\
	criAtomEx_SetDefaultConfig(&(p_config)->atom_ex);		\
	criAtomExAsr_SetDefaultConfig(&(p_config)->asr);		\
	criAtomExHcaMx_SetDefaultConfig(&(p_config)->hca_mx);	\
}

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/
/*==========================================================================
 *      CRI Atom API
 *=========================================================================*/

/*==========================================================================
 *      CRI AtomEx API
 *=========================================================================*/
/*JP
 * \brief Atomライブラリ初期化用コンフィグ構造体
 * \ingroup ATOMLIB_WASAPI
 * CRI Atomライブラリの動作仕様を指定するための構造体です。<br>
 * ::criAtomEx_Initialize_WASAPI 関数の引数に指定します。<br>
 * \sa criAtomEx_Initialize_WASAPI, criAtomEx_SetDefaultConfig_WASAPI
 */
typedef struct CriAtomExConfigTag_WASAPI {
	CriAtomExConfig			atom_ex;	/*JP< AtomEx初期化用コンフィグ構造体	*/
	CriAtomExAsrConfig		asr;		/*JP< ASR初期化用コンフィグ			*/
	CriAtomExHcaMxConfig	hca_mx;		/*JP< HCA-MX初期化用コンフィグ構造体	*/
} CriAtomExConfig_WASAPI;

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================
 *      CRI Atom API
 *=========================================================================*/



/*JP
 * \brief ミキサフォーマットの取得
 * \ingroup ATOMLIB_WASAPI
 * \param[out]	format		ミキサのフォーマット
 * \return		CriBool		ミキサのフォーマットが取得できたかどうか（ CRI_TRUE = 成功、CRI_FALSE = 失敗）
 * \par 説明:
 * 共有モード時に使用されるミキサのフォーマットを取得します。<br>
 * \par 備考:
 * 本関数は IAudioClient::GetMixFormat 関数のラッパーです。<br>
 * 本関数を実行すると、関数内で AudioClient を作成し、GetMixFormat 関数を実行します。<br>
 * \attention
 * 本関数はライブラリ初期化前にのみ使用可能です。<br>
 * <br>
 * 共有モードと排他モードとでは、使用できるフォーマットが異なります。<br>
 * 本関数で取得する WAVEFORMATEXTENSIBLE 構造体は IEEE float 形式のPCMデータフォーマットを返しますが、
 * このフォーマットは排他モードではほとんどの場合使用できません。<br>
 */
CriBool CRIAPI criAtom_GetAudioClientMixFormat_WASAPI(WAVEFORMATEXTENSIBLE *format);

/*JP
 * \brief 指定したフォーマットが利用可能かどうかチェック
 * \ingroup ATOMLIB_WASAPI
 * \param[in]	format		使用するフォーマット
 * \return		CriBool		指定されたフォーマットが利用可能かどうか（ CRI_TRUE = 利用可能、CRI_FALSE = 利用不可能）
 * \par 説明:
 * 引数で指定したフォーマットが、排他モードで利用可能かどうかをチェックします。<br>
 * \par 備考:
 * 本関数は IAudioClient::IsFormatSupported 関数のラッパーです。<br>
 * 本関数を実行すると、関数内で AudioClient を作成し、IsFormatSupported 関数を実行します。<br>
 * \attention
 * 本関数はライブラリ初期化前にのみ使用可能です。<br>
 * <br>
 * 一部のデバイス／パラメータについて、本関数が成功するにもかかわらず、
 * WASAPI の初期化に失敗するケースが確認されています。<br>
 * 本関数が CRI_TRUE を返したにもかかわらず、ライブラリの初期化に失敗する場合には、
 * 指定するフォーマットを変更するか、または共有モードをご使用ください。<br>
 */
CriBool CRIAPI criAtom_GetAudioClientIsFormatSupported_WASAPI(const WAVEFORMATEX *format);

/*JP
 * \brief 共有方式の指定
 * \ingroup ATOMLIB_WASAPI
 * \param[in]	mode	使用するモード
 * \par 説明:
 * WASAPIを共有モードで使用するか、排他モードで使用するかを指定します。<br>
 * <br>
 * 本関数を実行しない場合や、 AUDCLNT_SHAREMODE_SHARED を指定して実行した場合、
 * Atomライブラリは WASAPI を共有モードで初期化します。<br>
 * AUDCLNT_SHAREMODE_EXCLUSIVE を指定して本関数を実行した場合、
 * Atomライブラリは WASAPI を排他モードで初期化します。<br>
 * \attention
 * 本関数はライブラリの初期化よりも先に実行しておく必要があります。<br>
 * <br>
 * 排他モードを使用する場合、本関数でのモード指定に加え、
 * ::criAtom_SetAudioClientFormat_WASAPI 関数によるフォーマットの指定が必要です。<br>
 * \sa criAtom_SetAudioClientFormat_WASAPI
 */
void CRIAPI criAtom_SetAudioClientShareMode_WASAPI(AUDCLNT_SHAREMODE mode);

/*JP
 * \brief 出力フォーマットの指定
 * \ingroup ATOMLIB_WASAPI
 * \par 説明:
 * \param[in]	format		使用するフォーマット
 * \par 説明:
 * 排他モードで使用するフォーマットを指定します。<br>
 * \par 備考:
 * 本関数に指定したフォーマットが、 IAudioClient::Initialize 関数に渡されます。<br>
 * \attention
 * 本関数はライブラリの初期化よりも先に実行しておく必要があります。<br>
 * <br>
 * 排他モードを使用する場合、本関数でのモード指定に加え、
 * ::criAtom_SetAudioClientShareMode_WASAPI 関数によるモード指定が必要です。<br>
 * \sa criAtom_SetAudioClientShareMode_WASAPI
 */
void CRIAPI criAtom_SetAudioClientFormat_WASAPI(const WAVEFORMATEX *format);

/*JP
 * \brief バッファリング時間の指定
 * \ingroup ATOMLIB_WASAPI
 * \param[in]	ref_time	バッファリング時間
 * \par 説明:
 * WASAPI 初期化時に指定するバッファリング時間を指定します。<br>
 * Atomライブラリは、本関数で指定された時間分のデータを保持可能なサイズのサウンドバッファを確保します。<br>
 * \par 備考:
 * 本関数に指定した値が IAudioClient::Initialize 関数に渡されます。<br>
 * <br>
 * 本関数を使用しない場合、Atomライブラリは初期化時に指定されるサーバ処理周波数の値から、
 * 適切なバッファリング時間を計算します。<br>
 * \attention
 * 本関数はライブラリの初期化よりも先に実行しておく必要があります。<br>
 * <br>
 * バッファリング時間を小さくしすぎると、音途切れ等の問題が発生します。<br>
 */
void CRIAPI criAtom_SetAudioClientBufferDuration_WASAPI(REFERENCE_TIME ref_time);

/*JP
 * \brief AudioClientの取得
 * \ingroup ATOMLIB_WASAPI
 * \return IAudioClient	AudioClient
 * \par 説明:
 * Atomライブラリ内で作成されたAudioClientを取得します。
 * \par 備考:
 * サウンドデバイスが搭載されていないPCで本関数を実行した場合、
 * 本関数はNULLを返します。
 * \par 注意:
 * 本関数を実行する前に、ライブラリを初期化する必要があります。
 */
IAudioClient * CRIAPI criAtom_GetAudioClient_WASAPI(void);

/*JP
 * \brief デバイスが無効化されたかどうかのチェック
 * \ingroup ATOMLIB_WASAPI
 * \return	CriBool	デバイスが無効化されたかどうか（CRI_TRUE = 無効化された、CRI_FALSE = 正常に動作中）
 * \par 説明:
 * サウンドデバイスが無効化されたかどうかを返します。
 * \par 備考:
 * 本関数がCRI_TRUEを返すのは、アプリケーション実行中にサウンドデバイスを無効化した場合のみです。<br>
 * 元々サウンドデバイスが搭載されていないPCで本関数を実行した場合、本関数はCRI_FALSEを返します。<br>
 * （サウンドデバイスの有無は別途 ::criAtom_GetAudioClient_WASAPI 関数でチェックする必要があります。）
 * \sa criAtom_GetAudioClient_WASAPI
 */
CriBool CRIAPI criAtom_IsDeviceInvalidated_WASAPI(void);

/*==========================================================================
 *      CRI AtomEx API
 *=========================================================================*/
/*JP
 * \brief ライブラリ初期化用ワーク領域サイズの計算
 * \ingroup ATOMLIB_WASAPI
 * \param[in]	config		初期化用コンフィグ構造体
 * \return		CriSint32	ワーク領域サイズ
 * \par 説明:
 * ライブラリを使用するために必要な、ワーク領域のサイズを取得します。<br>
 * \par 備考:
 * ライブラリが必要とするワーク領域のサイズは、ライブラリ初期化用コンフィグ
 * 構造体（ ::CriAtomExConfig_WASAPI ）の内容によって変化します。<br>
 * <br>
 * 引数 config の情報は、関数内でのみ参照されます。<br>
 * 関数を抜けた後は参照されませんので、関数実行後に config の領域を解放しても
 * 問題ありません。
 * \sa CriAtomExConfig_WASAPI, criAtomEx_Initialize_WASAPI
 */
CriSint32 CRIAPI criAtomEx_CalculateWorkSize_WASAPI(const CriAtomExConfig_WASAPI *config);

/*JP
 * \brief ライブラリの初期化
 * \ingroup ATOMLIB_WASAPI
 * \param[in]	config		初期化用コンフィグ構造体
 * \param[in]	work		ワーク領域
 * \param[in]	work_size	ワーク領域サイズ
 * \par 説明:
 * ライブラリを初期化します。<br>
 * ライブラリの機能を利用するには、必ずこの関数を実行する必要があります。<br>
 * （ライブラリの機能は、本関数を実行後、 ::criAtomEx_Finalize_WASAPI 関数を実行するまでの間、
 * 利用可能です。）<br>
 * <br>
 * ライブラリを初期化する際には、ライブラリが内部で利用するためのメモリ領域（ワーク領域）
 * を確保する必要があります。<br>
 * ライブラリが必要とするワーク領域のサイズは、初期化用コンフィグ構造体の内容に応じて
 * 変化します。<br>
 * ワーク領域サイズの計算には、 ::criAtomEx_CalculateWorkSize_WASAPI 
 * 関数を使用してください。<br>
 * \par 備考:
 * ::criAtomEx_SetUserAllocator マクロを使用してアロケータを登録済みの場合、
 * 本関数にワーク領域を指定する必要はありません。<br>
 * （ work に NULL 、 work_size に 0 を指定することで、登録済みのアロケータ
 * から必要なワーク領域サイズ分のメモリが動的に確保されます。）
 * <br>
 * 引数 config の情報は、関数内でのみ参照されます。<br>
 * 関数を抜けた後は参照されませんので、関数実行後に config の領域を解放しても
 * 問題ありません。
 * \attention
 * 本関数は内部的に以下の関数を実行します。<br>
 * 	- ::criAtomEx_Initialize
 * 	- ::criAtomExAsr_Initialize
 * 	- ::criAtomExHcaMx_Initialize
 * 	.
 * 本関数を実行する場合、上記関数を実行しないでください。<br>
 * <br>
 * 本関数を実行後、必ず対になる ::criAtomEx_Finalize_WASAPI 関数を実行してください。<br>
 * また、 ::criAtomEx_Finalize_WASAPI 関数を実行するまでは、本関数を再度実行しないでください。<br>
 * \sa CriAtomExConfig_WASAPI, criAtomEx_Finalize_WASAPI,
 * criAtomEx_SetUserAllocator, criAtomEx_CalculateWorkSize_WASAPI
 */
void CRIAPI criAtomEx_Initialize_WASAPI(
	const CriAtomExConfig_WASAPI *config, void *work, CriSint32 work_size);

/*JP
 * \brief ライブラリの終了
 * \ingroup ATOMLIB_WASAPI
 * \par 説明:
 * ライブラリを終了します。<br>
 * \attention
 * 本関数は内部的に以下の関数を実行します。<br>
 * 	- ::criAtomEx_Finalize
 * 	- ::criAtomExAsr_Finalize
 * 	- ::criAtomExHcaMx_Finalize
 * 	.
 * 本関数を実行する場合、上記関数を実行しないでください。<br>
 * <br>
 * ::criAtomEx_Initialize_WASAPI 関数実行前に本関数を実行することはできません。<br>
 * \sa criAtomEx_Initialize_WASAPI
 */
void CRIAPI criAtomEx_Finalize_WASAPI(void);

#ifdef __cplusplus
}
#endif

#endif	/* CRI_INCL_CRI_ATOM_WASAPI_H */

/* --- end of file --- */
