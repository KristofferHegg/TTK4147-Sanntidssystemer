#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>

#include "io.h"


/// 'struct timespec' functions ///
struct timespec timespec_normalized(time_t sec, long nsec){
    while(nsec >= 1000000000){
        nsec -= 1000000000;
        ++sec;
    }
    while(nsec < 0){
        nsec += 1000000000;
        --sec;
    }
    return (struct timespec){sec, nsec};
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}


int set_cpu(int cpu_number){
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}


void* threadFunction(void *args){
	int a = *(int*)args;
	printf("Thread started with arg a=%d\n", a);

	set_cpu(1);

	while(1){
		if(!io_read(a)){
			io_write(a, 0);
			usleep(25);
			io_write(a, 1);
		}
	}
    return NULL;
}


void* threadDisturbance(void *args){
	set_cpu(1);

	while(1){
		asm volatile("" ::: "memory");
	}
    return NULL;
}


void* threadPeriodic(void *args){

	int a = *(int*)args;
	printf("Thread started with arg a=%d\n", a);

	struct timespec waketime;
	clock_gettime(CLOCK_REALTIME, &waketime);

	struct timespec period = {.tv_sec = 0, .tv_nsec = 1*1000*1000};

	while(1){
	    if(!io_read(a)){
			io_write(a, 0);
			usleep(25);
			io_write(a, 1);
		}
	    
	    // sleep
	    waketime = timespec_add(waketime, period);
	    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
	}
    return NULL;
}


int main(){

	printf("Running...\n");

	pthread_t thread1, thread2, thread3;
	pthread_t threadDist[10];
	pthread_t threadPeri[3];

	io_init();

	int args[3] = {1, 2, 3};

	/*
    pthread_create(&thread1, NULL, threadFunction, &args[0]);
	pthread_create(&thread2, NULL, threadFunction, &args[1]);
	pthread_create(&thread3, NULL, threadFunction, &args[2]);
	*/

	
	for(int i=0; i<10; i++){
		pthread_create(&threadDist[i], NULL, threadDisturbance, NULL);
	}
	

	for(int i=0; i<3; i++){
		pthread_create(&threadPeri[i], NULL, threadPeriodic,  &args[i]);
	}

	/*
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	*/
	pthread_join(threadPeri[0], NULL);

	return 0;
}
