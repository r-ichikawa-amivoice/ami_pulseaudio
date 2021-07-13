/*
 *
 * Copyright (C) 2021 Advanced Media, Inc. All Rights Reserved.
 *
 * This software is released under the [Apache License, Version 2.0]
 * see ../LICENSE
 *
 */
/*
 * @file ami_pulseaudio.h
 * @brief マルチスレッドにしたpulse_simpleラッパー
 * @author ichikawa
 * @date 20210319
 */


#ifndef __AMI_PULSEAUDIO_H_
#define __AMI_PULSEAUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @enum AMI_PULSEAUDIO_RESULT_STATE
 *  AMI_PULSEAUDIOのステータス
 */
enum AMI_PULSEAUDIO_STATE {
	//! ERROR
	AMI_PULSEAUDIO_STATE_ERROR = -1,
	//! セットアップ中
	AMI_PULSEAUDIO_STATE_SETUP = 0,
	//! セットアップ完了で待機中
	AMI_PULSEAUDIO_STATE_POUSE = 1,
	//! 録音中
	AMI_PULSEAUDIO_STATE_RECORDING = 2,
};

/callbackのステータス
/**
 *  @enum AMI_PULSEAUDIO_RESULT_STATE 
 *  callbackのステータス
 *  コールバックの処理はこの値を見てから処理をすると良い
 */
enum AMI_PULSEAUDIO_RESULT_STATE {
	//! エラー
	AMI_PULSEAUDIO_RESULT_STATE_ERROR = -1,
	//! ストップ
	AMI_PULSEAUDIO_RESULT_STATE_STOPPED = 0,
	//! スタート
	AMI_PULSEAUDIO_RESULT_STATE_STARTED = 1,
	//! 録音できたよ
	AMI_PULSEAUDIO_RESULT_STATE_ACCEPTED = 2,
};

/**
 * @typedef
 * 録音イベントのコールバック関数
 * @property (result_state) AMI_PULSEAUDIO_RESULT_STATE
 * @property (data_size) dataの長さ
 * @property (data) PCM 16kHz 1ch の音 
 */
typedef void (*ami_pulseaudio_callback_func)(enum AMI_PULSEAUDIO_RESULT_STATE result_state, int data_size, char* data);

/**
 * @fn
 * 初期化
 * @brief 録音変数の初期化
 * @param (callback) コールバック関数
 * @return 0:NG それ以外:録音変数のポインター
 */
void* ami_pulseaudio_create(ami_pulseaudio_callback_func callback);

/**
 * @fn
 * 破棄
 * @brief 録音変数の破棄
 * @param (ap) 録音変数
 * @return 0:NG 1:ok
 */
int ami_pulseaudio_free(void* ap);

/**
 * @fn
 * 録音開始
 * @brief 録音の開始
 * @param (ap) 録音変数
 * @return 0:NG 1:ok
 */
int ami_pulseaudio_start(void* ap);

/**
 * @fn
 * 録音終了
 * @brief 録音の終了
 * @param (ap) 録音変数
 * @return 0:NG 1:ok
 */
int ami_pulseaudio_stop(void* ap);

/**
 * @fn
 * 現在のステータス
 * @brief 現在のステータスの取得
 * @param (ap) 録音変数
 * @return AMI_PULSEAUDIO_STATE
 */
enum AMI_PULSEAUDIO_STATE ami_pulseaudio_get_state(void* ap);

#ifdef __cplusplus
}
#endif

#endif /* __AMI_PULSEAUDIO_H_ */
