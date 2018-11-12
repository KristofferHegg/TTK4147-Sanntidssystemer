#include <stdlib.h>
#include <stdio.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {

	int ch_id = ChannelCreate(0);
	int p_id = getpid();
	int rcv_id;
	char message[255];
	struct _msg_info info;

	FILE *fp;
	fp = fopen("/root/serverinfo.txt", "w");
	fprintf(fp, "%d %d", ch_id, p_id);
	fclose(fp);

	printf("Server started\n");

	/*// Task A
	while(1){
		printf("In while...\n");
		rcv_id = MsgReceive(ch_id, message, sizeof(message), NULL);
		printf("Got msg: %s", message);

		strcpy(message, "This is a reply");
		MsgReply(rcv_id, 0, message, sizeof(message));
		printf("Server reply\n");
	}
	*/

	// Task B
	while(1){
			printf("In while...\n");
			rcv_id = MsgReceive(ch_id, message, sizeof(message), &info);
			printf("Received message: %s\nProcess ID: %d, Thread ID: %d\n", message, info.pid, info.tid);

			strcpy(message, "This is a reply");
			MsgReply(rcv_id, 0, message, sizeof(message));
			printf("Server reply\n");
		}

	return EXIT_SUCCESS;
}
