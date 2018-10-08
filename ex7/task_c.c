#include <rtdk.h>
#include <sys/mman.h>
#include <stdio.h>
#include <native/timer.h>
#include <native/mutex.h>
#include <native/task.h>
#include <native/sem.h>

#define SEM_INIT 0
#define SEM_MODE S_FIFO
#define NUM_TASK 3

RT_SEM my_sem;
RT_MUTEX my_resource;

void busy_wait_us(unsigned long delay){
	for(; delay > 0; delay--){
		rt_timer_spin(1000);
	}
}

void my_func(void *args) {
	int a = *(int*)args;
	rt_printf("Task %d - Waiting to start\n", a);

	rt_sem_p(&my_sem, TM_INFINITE);
	rt_printf("Task %d - Started\n", a);

	if(a==1){
		rt_printf("Task %d - Waiting for a mutex\n", a);
		rt_mutex_acquire(&my_resource,TM_INFINITE);
		rt_printf("Task %d - Got a mutex\n", a);
		busy_wait_us(300000);
		rt_printf("Task %d - Released a mutex\n", a);
		rt_mutex_release(&my_resource);
	} else if(a==2) {		
		rt_task_sleep(100000000);
		busy_wait_us(500000);
	} else if(a==3){		
		rt_task_sleep(200000000);
		rt_printf("Task %d - Waiting for a mutex\n", a);
		rt_mutex_acquire(&my_resource,TM_INFINITE);
		rt_printf("Task %d - Got a mutex\n", a);
		busy_wait_us(200000);
		rt_printf("Task %d - Released a mutex\n", a);
		rt_mutex_release(&my_resource);
	}

	rt_sem_v(&my_sem);
	rt_printf("Task %d - Finished\n", a);
}

void my_unlock_func(void *args) {
	rt_task_sleep(100000000);
	rt_sem_broadcast(&my_sem);
}

int main(){
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);
	
	RT_TASK my_unlock_task;
	RT_TASK my_task[NUM_TASK];
	int my_task_prio[NUM_TASK] = {40,50,60};
	int my_task_args[NUM_TASK] = {1,2,3};

	rt_sem_create(&my_sem, NULL, SEM_INIT, SEM_MODE);
	rt_mutex_create(&my_resource,"MyMutex");

	rt_task_create(&my_unlock_task, NULL, 0, 70, T_CPU(1) | T_JOINABLE);
	rt_task_start(&my_unlock_task, &my_unlock_func, NULL);

	for(int i=0; i < NUM_TASK; i++){
		rt_task_create(&my_task[i], NULL, 0, my_task_prio[i], T_CPU(1) | T_JOINABLE);
	}

	for(int j=0; j < NUM_TASK; j++){	
		rt_task_start(&my_task[j], &my_func, &my_task_args[j]);
	}

	for(int i = 0; i < NUM_TASK; i++){
		rt_task_join(&my_task[i]);	
	}
	
	rt_task_join(&my_unlock_task);

	rt_task_sleep(100000000);
	
	rt_sem_delete(&my_sem);
	rt_mutex_delete(&my_resource);
	
	return 0;
}
