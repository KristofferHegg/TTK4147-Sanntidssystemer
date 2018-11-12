#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/dispatch.h>
#include <pthread.h>

void* message_client(void* arg){
	char receive_buf[64];
	char send_buf[64];
	memset(receive_buf, 0, 64);
	//Find channel
	FILE* file = fopen("/root/serverinfo.txt", "r");
	char proc_id_str[12];
	char channel_id_str[10];
	fscanf(file, "%s %s", proc_id_str, channel_id_str);
	printf("test: %s, %s\n", proc_id_str, channel_id_str);
	fclose(file);

	int proc_id = atoi(proc_id_str);
	int channel_id = atoi(channel_id_str);

	int conn_id = ConnectAttach(0, proc_id, channel_id, 0, 0);
	sprintf(send_buf, "Client says hello");

	MsgSend(conn_id, send_buf, 64, receive_buf, 64);

	printf("Reply: %s\n", receive_buf);
	ConnectDetach(conn_id);
	return 0;
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
	set_priority(60);
	printf("priority after update: %d\n", get_priority());

	pthread_t c1;
	pthread_t c2;
	pthread_t c3;
	pthread_t c4;

	pthread_create(&c1, set_priority(10), message_client, NULL);
	pthread_create(&c2, set_priority(15), message_client, NULL);
	pthread_create(&c3, set_priority(25), message_client, NULL);
	pthread_create(&c4, set_priority(30), message_client, NULL);

	pthread_join(c1, NULL);
	pthread_join(c2, NULL);
	pthread_join(c3, NULL);
	pthread_join(c4, NULL);

	return EXIT_SUCCESS;
}
