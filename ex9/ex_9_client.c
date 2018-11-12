#include <stdlib.h>
#include <stdio.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {

	int connection_id = 0;
	int ch_id = 1;
	int p_id = 1421349;
	char *smsg = "This is a message.\n";
	char rmsg[255];

	char ch_id_buf[255];
	char p_id_buf[255];

	printf("Client started\n");


	// Get process and channel id
	FILE *fp;
	fp = fopen("/root/serverinfo.txt", "r");
	fscanf(fp, "%s %s", ch_id_buf, p_id_buf);
	fclose(fp);

	ch_id = atoi(ch_id_buf);
	p_id = atoi(p_id_buf);

	// Connect
	connection_id = ConnectAttach(0, p_id, ch_id, 0, 0);

	if(connection_id == -1){
		printf("Couldnt connect to server...\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	printf("Client connected\n");

	if(MsgSend(connection_id, smsg, strlen(smsg)+1, rmsg, sizeof(rmsg) )) {
		printf("Error sending msg...\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	printf("Client sendt\n");

	if(strlen(rmsg) > 0) {
		printf("Got msg: %s", rmsg);
	}
	printf("Client recieved\n");

	// Disconnect
	ConnectDetach(connection_id);

	printf("Client disconnected\n");

	return EXIT_SUCCESS;
}
