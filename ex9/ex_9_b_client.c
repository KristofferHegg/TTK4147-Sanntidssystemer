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


int main(int argc, char *argv[]) {
	pthread_t c1;
	pthread_t c2;
	pthread_t c3;
	pthread_t c4;

	pthread_create(&c1, NULL, message_client, NULL);
	pthread_create(&c2, NULL, message_client, NULL);
	pthread_create(&c3, NULL, message_client, NULL);
	pthread_create(&c4, NULL, message_client, NULL);

	pthread_join(c1, NULL);
	pthread_join(c2, NULL);
	pthread_join(c3, NULL);
	pthread_join(c4, NULL);

	return EXIT_SUCCESS;
}
