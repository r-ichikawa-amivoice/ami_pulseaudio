/*
 *
 * Copyright (C) 2021 Advanced Media, Inc. All Rights Reserved.
 *
 * This software is released under the [Apache License, Version 2.0]
 * see ../LICENSE
 *
 * build command
 * $gcc ami_pulseaudio_core.cpp -I./ -lpulse -lpulse-simple -lpthread -fPIC -shared -o libami_pulseaudio.so
 *
 * build command(debug)
 * $gcc ami_pulseaudio_core.cpp -I./ -lpulse -lpulse-simple -lpthread -fPIC -shared -o libami_pulseaudio.so -DAMI_PULSEAUDIO_LOG=0
 *
 */
/*
 * @file ami_pulseaudio_core.cpp
 * @brief マルチスレッドにしたpulse_simpleラッパー
 * @author ichikawa
 * @date 20210319
 */

#include "ami_pulseaudio_core.h"

void ami_pulseaudio_printf(int log_level, const char* format, ...){
	if(AMI_PULSEAUDIO_LOG > log_level) return;
	va_list va;
	va_start(va, format);
	switch(log_level){
		case AMI_PULSEAUDIO_LOG_DEBUG:
			printf("DEBUG   : ");
			break;
		case AMI_PULSEAUDIO_LOG_INFO:
			printf("INFO    : ");
			break;
		case AMI_PULSEAUDIO_LOG_WORNING:
			printf("WORNING : ");
			break;
		case AMI_PULSEAUDIO_LOG_ERROR:
			printf("ERROR   : ");
			break;
	}
	vprintf(format, va);
	va_end(va);	
}

void ami_pulseaudio_change_state(void* ap, enum AMI_PULSEAUDIO_STATE state){
	ami_pulseaudio_t* ap_ = (ami_pulseaudio_t*)ap;

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_change_state ->\n");

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_INFO, "change state: %d => %d\n", ap_->state, state);
	ap_->state = state;

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_change_state <-\n");
}

void* ami_pulseaudio_read(void* ap){
	ami_pulseaudio_t* ap_ = (ami_pulseaudio_t*)ap;
	int pa_errno, pa_result;

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_read ->\n");

	ami_pulseaudio_change_state((void*)ap_, AMI_PULSEAUDIO_STATE_RECORDING);

	ap_->callback(AMI_PULSEAUDIO_RESULT_STATE_STARTED, 0, 0);

	while(ap_->instruction){
		pa_result = pa_simple_read(ap_->ps, ap_->audio_data, AMI_PULSEAUDIO_DATA_SIZE, &pa_errno);	
		if (pa_result < 0) {
			ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_ERROR, "ami_pulseaudio_read: read data from pulseaudio: %s\n", pa_strerror(pa_errno));
			ap_->callback(AMI_PULSEAUDIO_RESULT_STATE_ERROR, 0, 0);
			ap_->instruction = 0;
		}
		ap_->callback(AMI_PULSEAUDIO_RESULT_STATE_ACCEPTED, AMI_PULSEAUDIO_DATA_SIZE, ap_->audio_data);
	}
	ap_->callback(AMI_PULSEAUDIO_RESULT_STATE_STOPPED, 0, 0);

	ami_pulseaudio_change_state((void*)ap_, AMI_PULSEAUDIO_STATE_POUSE);
	
	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_read <-\n");
}


void* ami_pulseaudio_create(ami_pulseaudio_callback_func callback){
	ami_pulseaudio_t* ap_;
	int is_setup_complete = 0;
	int pa_errno;
	
	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_create ->\n");

	//領域確保
	ap_ = (ami_pulseaudio_t*)calloc(1, sizeof(ami_pulseaudio_t));
	if(ap_ == NULL){
		ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_ERROR, "ami_pulseaudio_create: allocate memory\n");
		ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_create <-\n");
		return 0;
	}

	//ステータスをセットアップ状態に
	ami_pulseaudio_change_state(ap_, AMI_PULSEAUDIO_STATE_SETUP);

	//色々セッティング
	do{
		pa_sample_spec ss;

		ap_->callback=callback;

		//16000khz 16bit ch1
		ss.format = PA_SAMPLE_S16LE;
		ss.rate = 16000;
		ss.channels = 1;
		ap_->ps = pa_simple_new(NULL, AMI_PULSEAUDIO_APP_NAME, PA_STREAM_RECORD, NULL, AMI_PULSEAUDIO_STREAM_NAME, &ss, NULL, NULL, &pa_errno);
		if (ap_->ps == NULL) {
			ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_ERROR, "ami_pulseaudio_create: connect pulseaudio server: %s\n", pa_strerror(pa_errno));
			break;
		}
		is_setup_complete = 1;
	}while(0);

	//初期化出来ていなかったら0を返す
	if(!is_setup_complete){
		ami_pulseaudio_change_state(ap_, AMI_PULSEAUDIO_STATE_ERROR);

		ami_pulseaudio_free(ap_);
		ap_ = 0;

		ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_ERROR, "Failed to setup ap\n");
	}else{

		//オッケーそうだからステータスを待機に
		ami_pulseaudio_change_state(ap_, AMI_PULSEAUDIO_STATE_POUSE);

	}

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_INFO, "ami_pulseaudio_create: create OK!\n");

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_create <-\n");

	return ap_;
}

int ami_pulseaudio_free(void* ap){
	ami_pulseaudio_t* ap_ = (ami_pulseaudio_t*)ap;
	int result = 0;

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_free ->\n");
	
	do{
		if(ap_ == 0){
			ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_WORNING, "ami_pulseaudio_free: NULL ptr\n");
			break;
		}
		ap_->instruction = 0;
		pthread_join(ap_->read_thread, NULL);

		pa_simple_free(ap_->ps);
		free(ap_);
		ap_ = 0;
		result = 1;
	}while(0);

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_INFO, "ami_pulseaudio_free: free OK!\n");

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_free <-\n");
	return result;
}

int ami_pulseaudio_start(void* ap){
	ami_pulseaudio_t* ap_ = (ami_pulseaudio_t*)ap;
	int result = 0;

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_start ->\n");

	do{
		enum AMI_PULSEAUDIO_STATE state;
		int instruction;
		if(ap_ == 0){
			ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_WORNING, "ami_pulseaudio_start: can not start: NULL ptr\n");
			break;
		}
		state = ap_->state;
		instruction = ap_->instruction;
		if(state != AMI_PULSEAUDIO_STATE_POUSE){
			ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_WORNING, "ami_pulseaudio_start: can not start : state: %d\n", state);
			break;
		}
		if(instruction != 0){
			ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_WORNING, "ami_pulseaudio_start: can not start: instruction: %d\n", instruction);
			break;
		}

		ap_->instruction = 1;
		pthread_create(&ap_->read_thread, NULL, &ami_pulseaudio_read, (void*)ap_);
		result = 1;
	}while(0);

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_start <-\n");

	return result;
}

int ami_pulseaudio_stop(void* ap){
	ami_pulseaudio_t* ap_ = (ami_pulseaudio_t*)ap;
	int result = 0;

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_stop ->\n");
	
	do{
		enum AMI_PULSEAUDIO_STATE state;
		int instruction;
		if(ap_ == 0){
			ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_WORNING, "ami_pulseaudio_stop: NULL ptr\n");
			break;
		}
		state = ap_->state;
		instruction = ap_->instruction;
		if(state != AMI_PULSEAUDIO_STATE_RECORDING){
			ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_WORNING, "ami_pulseaudio_stop: can not stop : state: %d\n", state);
			break;
		}
		if(instruction == 0){
			ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_WORNING, "ami_pulseaudio_stop: can not stop : instruction: %d\n", instruction);
			break;
		}
	
		ap_->instruction = 0;
		result = 1;
	}while(0);

	ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_DEBUG, "ami_pulseaudio_stop <-\n");

	return result;
}

enum AMI_PULSEAUDIO_STATE ami_pulseaudio_get_state(void* ap){
	ami_pulseaudio_t* ap_ = (ami_pulseaudio_t*)ap;
	int result = 0;

	do{
		if(ap_ == 0){
			ami_pulseaudio_printf(AMI_PULSEAUDIO_LOG_ERROR, "ami_pulseaudio_get_state: NULL ptr\n");
			result = AMI_PULSEAUDIO_STATE_ERROR;
			break;
		}
		result = ap_->state;
	}while(0);
	
	return (AMI_PULSEAUDIO_STATE)result;
}
