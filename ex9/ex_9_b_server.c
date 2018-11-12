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
	int channel_id = ChannelCreate(0);
	if (channel_id == -1){
		printf("Error in creating channel");
	}
	pid_t proc_id = getpid();
	FILE* file = fopen("/root/serverinfo.txt", "w");
	fprintf(file, "%d %d\n", proc_id, channel_id);
	fclose(file);

	while(1){
		int rec_id = MsgReceive(channel_id, receive_buf, 64, &info);
		printf("Received message: %s\nProcess ID: %d, Thread ID: %d\n", receive_buf, info.pid, info.tid);
		MsgReply(rec_id, 0, send_buf, 64);
	}
}

int main(int argc, char *argv[]) {
	message_server();
	return EXIT_SUCCESS;
}
