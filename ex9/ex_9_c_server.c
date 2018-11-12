#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/dispatch.h>

void message_server(void){
	char receive_buf[64];
	char send_buf[64];
	struct _msg_info info;
	memset(receive_buf, 0, 64);
	sprintf(send_buf, "Message received in server.");
	//int channel_id = ChannelCreate(_NTO_CHF_FIXED_PRIORITY);
	int channel_id = ChannelCreate(NULL);
	if (channel_id == -1){
		printf("Error in creating channel");
	}
	pid_t proc_id = getpid();
	FILE* file = fopen("/root/serverinfo.txt", "w");
	fprintf(file, "%d %d\n", proc_id, channel_id);
	fclose(file);

	while(1){
		int rec_id = MsgReceive(channel_id, receive_buf, 64, &info);
		printf("Received message: %s\nProcess ID: %d, Thread ID: %d, Priority: %d\n", receive_buf, info.pid, info.tid, info.priority);
		MsgReply(rec_id, 0, send_buf, 64);
	}
}

int set_priority(int priority){
    int policy;
    struct sched_param param;

    if(priority < 1 || priority > 63){
        return -1;
    }

    pthread_getschedparam(pthread_self(), &policy, &param);
    param.sched_priority = priority;
    return pthread_setschedparam(pthread_self(), policy, &param);
}

int get_priority(){
    int policy;
    struct sched_param param;
    pthread_getschedparam(pthread_self(), &policy, &param);
    return param.sched_curpriority;
}


int main(int argc, char *argv[]) {
	printf("priority before update: %d\n", get_priority());
	set_priority(20);
	printf("priority after update: %d\n", get_priority());
	message_server();
	return EXIT_SUCCESS;
}
