/*
 *
 * Copyright (C) 2021 Advanced Media, Inc. All Rights Reserved.
 *
 * This software is released under the [Apache License, Version 2.0]
 * see ../LICENSE
 *
 */
/*
 * @file ami_pulseaudio_core.cpp
 * @brief マルチスレッドにしたpulse_simpleラッパー
 * @author ichikawa
 * @date 20210319
 */

#ifndef __AMI_PULSEAUDIO_CORE_H_
#define __AMI_PULSEAUDIO_CORE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pulse/error.h> /* pulseaudio */
#include <pulse/simple.h> /* pulseaudio */
#include <pthread.h>
#include "ami_pulseaudio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AMI_PULSEAUDIO_APP_NAME "ami_pulseaudio_in"
#define AMI_PULSEAUDIO_STREAM_NAME "rec"

#define AMI_PULSEAUDIO_DATA_SIZE 512

#ifndef AMI_PULSEAUDIO_LOG
#define AMI_PULSEAUDIO_LOG 5
#endif

enum ami_pulseaudio_log_level {
	AMI_PULSEAUDIO_LOG_DEBUG,
	AMI_PULSEAUDIO_LOG_INFO,
	AMI_PULSEAUDIO_LOG_WORNING,
	AMI_PULSEAUDIO_LOG_ERROR,
};

//必要なもの入れた構造体
typedef struct{
	//ステータス情報
	enum AMI_PULSEAUDIO_STATE state;
	//audio_data
	//どうせ1対1で必要だから中に入れちゃう
	char audio_data[AMI_PULSEAUDIO_DATA_SIZE];
	//pulseaudioの変数
	pa_simple* ps;
	//コールバック関数はここに入れとく
	ami_pulseaudio_callback_func callback;
	//録音の指示フラグ
	int instruction = 0;
	//threadの変数
	pthread_t read_thread;
}ami_pulseaudio_t;

void ami_pulseaudio_printf(int log_level, const char* format, ...);

void ami_pulseaudio_change_state(void* ap, enum AMI_PULSEAUDIO_STATE state);

void* ami_pulseaudio_read(void* ap);

#ifdef __cplusplus
}
#endif

#endif /* __AMI_PULSEAUDIO_CORE_H_ */
