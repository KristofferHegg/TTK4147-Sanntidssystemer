#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h>
#include <rtdk.h>
#include "io.h"
#include <pthread.h>

RT_TASK hello_task;

void helloWorld(void *arg) {
	rt_task_set_periodic(&hello_task, TM_NOW, 1*1000*1000*1000);
	
	while(1){
		printf("Hello World!\n");
		rt_task_wait_period(NULL);
	}
}

// Set single CPU for pthread threads (NOT Xenomai threads!)
int set_cpu(int cpu_number){
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void busy_wait(void *args) {

	unsigned long duration = 30000000000; // 10 second timeout
	unsigned long endTime = rt_timer_read() + duration;

	int a = *(int*)args;
	
	rt_printf("Startet task %d\n", a);
	
	while(1){
		
		if(!io_read(a)) {
			io_write(a, 0);
			io_write(a, 1);  
		}

		if(rt_timer_read() > endTime){
				rt_printf("Time expired %d\n", a);
				rt_task_delete(NULL);
		}
		if(rt_task_yield()){
				rt_printf("Task failed to yield %d\n", a);
				rt_task_delete(NULL);
		}
	}
}

void xenomai_task(void *args) {
	rt_task_set_periodic(NULL, TM_NOW, 1*1000*1000);

	int a = *(int*)args;
	
	rt_printf("Startet task %d\n", a);
	
	while(1){
		if(!io_read(a)) {
			io_write(a, 0);
			io_write(a, 1);  
		}
		rt_task_wait_period(NULL);
	}
}

void* threadDisturbance(void *args){
	set_cpu(1);
	while(1){
		asm volatile("" ::: "memory");
	}
	return NULL;
}

int main() {

	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);

	io_init();
	
	// Create three tasks
	RT_TASK tread_busy_wait[3];
	RT_TASK tread_xenomai[3];
	int args[3] = {1,2,3};

	// Create ten disturbance tasks
	pthread_t threadDist[10];

	char  str[10];
	sprintf(str,"hello");
	
	/* Hello World test
	rt_task_create(&hello_task, str, 0, 50, 0);
	rt_task_start(&hello_task, &helloWorld, 0);
	*/
	
	// Task A
	/*
	for(int i=0; i<3; i++){
		rt_task_create(&tread_busy_wait[i], NULL, 0, 50, T_CPU(1));
	}

	for(int i=0; i<3; i++){
		rt_task_start(&tread_busy_wait[i], &busy_wait, &args[i]);
	}*/
	/*
	for(int i=0; i<10; i++){
		pthread_create(&threadDist[i], NULL, threadDisturbance, NULL);
	}*/

	// Task B
	for(int i=0; i<3; i++){
		rt_task_create(&tread_xenomai[i], NULL, 0, 50, T_CPU(1));
	}

	for(int i=0; i<3; i++){
		rt_task_start(&tread_xenomai[i], &xenomai_task, &args[i]);
	}

	while(1);

	return 0;
}
