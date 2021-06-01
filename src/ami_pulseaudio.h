#ifndef __AMI_PULSEAUDIO_H_
#define __AMI_PULSEAUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

//ステータス
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

//callbackのステータス
enum AMI_PULSEAUDIO_RESULT_STATE {
	//エラー
	AMI_PULSEAUDIO_RESULT_STATE_ERROR = -1,
	//ストップ
	AMI_PULSEAUDIO_RESULT_STATE_STOPPED = 0,
	//スタート
	AMI_PULSEAUDIO_RESULT_STATE_STARTED = 1,
	//録音できたよ
	AMI_PULSEAUDIO_RESULT_STATE_ACCEPTED = 2,
};

typedef void (*ami_pulseaudio_callback_func)(enum AMI_PULSEAUDIO_RESULT_STATE result_state, int data_size, char* data);

//初期化
//0:ng
//それ以外:ポインター
void* ami_pulseaudio_create(ami_pulseaudio_callback_func callback);

//破棄
//0:ng
//1:ok
int ami_pulseaudio_free(void* ap);

//録音開始
//0:ng
//1:ok
int ami_pulseaudio_start(void* ap);

//録音終了
//0:ng
//1:ok
int ami_pulseaudio_stop(void* ap);

//現在のステータス
enum AMI_PULSEAUDIO_STATE ami_pulseaudio_get_state(void* ap);

#ifdef __cplusplus
}
#endif

#endif /* __AMI_PULSEAUDIO_H_ */
