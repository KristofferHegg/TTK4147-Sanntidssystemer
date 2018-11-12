#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "time/time.h"
#include "network/udp.h"
#include "pi.h"


void nanowait(int sec, int nanosec){
 
    struct timespec waketime;
    clock_gettime(CLOCK_REALTIME, &waketime);

    struct timespec period = {.tv_sec=sec, .tv_nsec=nanosec};

    waketime = timespec_add(waketime, period);

    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);    
}


int main()
{
	UDPConn* conn = udpconn_new("192.168.0.1", 9999);
    char sendBuf[64];
    char recvBuf[64];    
    memset(recvBuf, 0, sizeof(recvBuf));
    
    // Start server
    sprintf(sendBuf, "START");    
    udpconn_send(conn, sendBuf);
    printf("\n\n******** Server started ********\n\n");

    char piBuf[64]; 
    char strBuf[64]; 

    float sp = 0.0;
    float dt = 0.01;

    int i;
    for(i=0; i<200; i++){
        
        sprintf(sendBuf, "GET");
        udpconn_send(conn, sendBuf);

        udpconn_receive(conn, recvBuf, sizeof(recvBuf));
        strncpy(strBuf, recvBuf+8, 4);

        // Set setpoint
        if(i < 100){            
            sp = 1.0;
        } else{        
            sp = 0.0;    
        }
        
        sprintf(piBuf, "%f", PI(sp, strtod(strBuf, NULL), dt));

        sprintf(sendBuf, "SET:%s", piBuf);
        udpconn_send(conn, sendBuf);

        nanowait(0, 10000000);
    }
    
    sprintf(sendBuf, "STOP");    
    udpconn_send(conn, sendBuf);
    
    udpconn_delete(conn);

    printf("Server stopped\n\n");
    
	return 0;
}