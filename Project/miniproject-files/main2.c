

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "time/time.h"
#include "network/udp.h"
#include "pid.h"

int isFinished = 0;


void* threadUpdateSetpoint(void *input){

	PID_controller* c = (PID_controller*)input;

	pid_set_setpoint(c, 1.0);
	nanowait(1,0);

	pid_set_setpoint(c, 0.0);
	nanowait(1,0);

	isFinished = 1;

	return NULL;
}


void* threadPid(void *input){

	PID_controller* c = (PID_controller*)input;

	struct timespec waketime;
	clock_gettime(CLOCK_REALTIME, &waketime);
	struct timespec period = {.tv_sec = 0, .tv_nsec = 50000000};

	while(1){

	    pid_calculate_control_signal(c);

	    // sleep
	    waketime = timespec_add(waketime, period);
	    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
	}

    return NULL;
}


int main(){

	// Connect to server
	UDPConn* conn = udpconn_new("192.168.0.1", 9999); 

	// Create buffers   
	char sendBuf[64];
    char recvBuf[64];    
	char ackSubStr[] = "GET_ACK:";
	char signalStr[] = "SIGNAL";
    memset(recvBuf, 0, strlen(recvBuf));

    // Start server
    sprintf(sendBuf, "START");    
    udpconn_send(conn, sendBuf);
    printf("\n\n******** Server started ********\n\n");

	pthread_t thread_pid, thread_setpoint;

	PID_controller* valvePID = pid_new(5.0,800.0,0.0,0.05);

	pthread_create(&thread_pid, NULL, threadPid, (void*)valvePID);
	pthread_create(&thread_setpoint, NULL, threadUpdateSetpoint, (void*)valvePID);


	while(!isFinished){

		sprintf(sendBuf, "GET");
        udpconn_send(conn, sendBuf);

        udpconn_receive(conn, recvBuf, sizeof(recvBuf));	

        //printf("%d\n", (strcmp(recvBuf,signalStr)));
		
		//printf("%s\n", recvBuf);

		if (strcmp(recvBuf,signalStr) == 0 ) {
			//printf("%s\n", recvBuf);
			sprintf(sendBuf, "SIGNAL_ACK");
			//memset(sendBuf, 0, strlen(sendBuf));
		} else {
			pid_set_process_value(valvePID, atof(recvBuf + strlen(ackSubStr)));
			sprintf(sendBuf, "SET:%f", pid_get_control_signal(valvePID));
			//printf("PID: %s\n", sendBuf);
		}

        udpconn_send(conn, sendBuf);
	}

	sprintf(sendBuf, "STOP");    
    udpconn_send(conn, sendBuf);
    
    udpconn_delete(conn);

    printf("Server stopped\n\n");

    return 0;
}
