#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "time/time.h"
#include "network/udp.h"
#include "pid.h"

// Connect to server
UDPConn* conn;

// Create buffers   
char sendBuf[64];
char recvBuf[64];

sem_t semaphore;
pthread_mutex_t mutex_pid;

void* threadUpdateSetpoint(void *input){

	PID_controller* c = (PID_controller*)input;

	pid_set_setpoint(c, 1.0);
	wait_sec_nanosec(1,0);

	pid_set_setpoint(c, 0.0);
	wait_sec_nanosec(1,0);

	return NULL;
}


void* getSender(void* argp){

	struct timespec waketime;
	clock_gettime(CLOCK_REALTIME, &waketime);
	struct timespec period = {.tv_sec = 0, .tv_nsec = 5000000};

	while(1){
		udpconn_send(conn, "GET");
		
		// sleep
	    waketime = timespec_add(waketime, period);
	    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
	}
}

void* threadPid(void *input){

	PID_controller* c = (PID_controller*)input;

	while(1){

        sem_wait(&semaphore);
        pthread_mutex_lock(&mutex_pid);
	    pid_calculate_control_signal(c);
	    
	    // Set process value to PID controller
	    sprintf(sendBuf, "SET:%f", pid_get_control_signal(c));
	    pthread_mutex_unlock(&mutex_pid);
        udpconn_send(conn, sendBuf);
	}

    return NULL;
}

void* threadUdpRecieve(void *input){
	
	PID_controller* c = (PID_controller*)input;
	
	char signalStr[] = "SIGNAL";
	char ackSubStr[] = "GET_ACK:";

	while(1){
		udpconn_receive(conn, recvBuf, sizeof(recvBuf));

		if (strcmp(recvBuf,signalStr) == 0 ) {
			sprintf(sendBuf, "SIGNAL_ACK");
			udpconn_send(conn, sendBuf);
		} else {
			pthread_mutex_lock(&mutex_pid);
			pid_set_process_value(c, atof(recvBuf + strlen(ackSubStr)));
			pthread_mutex_unlock(&mutex_pid);
			//printf("%s\n", recvBuf);
			sem_post(&semaphore);
		} 
	}
}

int main(){

	conn = udpconn_new("192.168.0.1", 9999); 
	
	memset(recvBuf, 0, strlen(recvBuf));

    // Start server
    sprintf(sendBuf, "START");    
    udpconn_send(conn, sendBuf);
    printf("\n\n******** Server started ********\n\n");

    sem_init(&semaphore, 0, 0);
    pthread_mutex_init(&mutex_pid, NULL);

	pthread_t thread_pid, thread_setpoint, thread_udp_receive, thread_get;

	PID_controller* valvePID = pid_new(1.0,100000.0,0.0,0.005);

	pthread_create(&thread_udp_receive, NULL, threadUdpRecieve, (void*)valvePID);
	pthread_create(&thread_pid, NULL, threadPid, (void*)valvePID);
	pthread_create(&thread_setpoint, NULL, threadUpdateSetpoint, (void*)valvePID);
	pthread_create(&thread_get, NULL, getSender, NULL);

	pthread_join(thread_setpoint, NULL);

	sprintf(sendBuf, "STOP");    
    udpconn_send(conn, sendBuf);
    
    udpconn_delete(conn);

    sem_destroy(&semaphore);
    pthread_mutex_destroy(&mutex_pid);

    printf("Server stopped\n\n");

    return 0;
}
