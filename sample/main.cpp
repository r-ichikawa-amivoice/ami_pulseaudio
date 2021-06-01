#include "ami_pulseaudio.h"
#include <stdio.h>
#include <unistd.h>

void callback(enum AMI_PULSEAUDIO_RESULT_STATE result_state, int data_size, char* data){
#if 1
	switch(result_state){
		case AMI_PULSEAUDIO_RESULT_STATE_ACCEPTED:
			write(STDOUT_FILENO, data, data_size);
			break;
	}
#else
	switch(result_state){
		case AMI_PULSEAUDIO_RESULT_STATE_ERROR:
			printf("CALLBACK: AMI_PULSEAUDIO_RESULT_STATE_ERROR\n");
			break;
		case AMI_PULSEAUDIO_RESULT_STATE_STOPPED:
			printf("CALLBACK: AMI_PULSEAUDIO_RESULT_STATE_STOPPED\n");
			break;
		case AMI_PULSEAUDIO_RESULT_STATE_STARTED:
			printf("CALLBACK: AMI_PULSEAUDIO_RESULT_STATE_STARTED\n");
			break;
		case AMI_PULSEAUDIO_RESULT_STATE_ACCEPTED:
			printf("CALLBACK: AMI_PULSEAUDIO_RESULT_STATE_ACCEPTED\n");
			break;
	}
#endif
}

int main(int argc, char** argv) {
	void* ap ;
	ap = ami_pulseaudio_create(callback);
	ami_pulseaudio_start(ap);
	getchar();
	ami_pulseaudio_stop(ap);
	ami_pulseaudio_free(ap);
	ap = 0;
}
