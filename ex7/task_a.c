#include <rtdk.h>
#include <native/task.h>
#include <native/sem.h>
#include <sys/mman.h>
#include <stdio.h>

#define SEM_INIT 0
#define SEM_MODE S_FIFO

RT_SEM my_sem;

void my_func(void *args) {
	int a = *(int*)args;
	rt_printf("Task %d - Waiting for a semaphore\n", a);

	rt_sem_p(&my_sem, TM_INFINITE);
	rt_printf("Task %d - Got a semaphore\n", a);
	rt_task_sleep(100000000);

	rt_sem_v(&my_sem);
	rt_printf("Task %d - Released a semaphore\n", a);
}

void my_unlock_func(void *args) {
	rt_task_sleep(100000000);
	rt_sem_broadcast(&my_sem);
}

int main(){
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);
	
	RT_TASK my_unlock_task;
	RT_TASK my_task[2];
	int my_task_prio[2] = {40,50};
	int my_task_args[2] = {1,2};

	rt_sem_create(&my_sem, NULL, SEM_INIT, SEM_MODE);

	rt_task_create(&my_unlock_task, NULL, 0, 60, T_CPU(1) | T_JOINABLE);
	rt_task_start(&my_unlock_task, &my_unlock_func, NULL);

	for(int i=0; i < 2; i++){
		rt_task_create(&my_task[i], NULL, 0, my_task_prio[i], T_CPU(1) | T_JOINABLE);
	}

	for(int j=0; j < 2; j++){	
		rt_task_start(&my_task[j], &my_func, &my_task_args[j]);
	}

	for(int i = 0; i < 2; i++){
		rt_task_join(&my_task[i]);	
	}
	
	rt_task_join(&my_unlock_task);

	rt_task_sleep(100000000);
	
	rt_sem_delete(&my_sem);
	
	return 0;
}
