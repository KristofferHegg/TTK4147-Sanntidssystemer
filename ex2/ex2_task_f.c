#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t forks[5];
pthread_t philosopher[5];


void think(int position)
{
  printf("Philosopher %d thinking...\n", position);
  sleep(2);
}

void eat(int position)
{
  printf("Philosopher %d eating...\n", position);
  //sleep(2);
}

int left_forks(int i)
{
	return i;
}

int right_forks(i)
{
	return (i+1) % 5;
}

void get_forks(i)
{	
	//Left handed philosopher
	if(i==4){		
		pthread_mutex_lock(&forks[left_forks(i)]); 
		usleep(1);
		pthread_mutex_lock(&forks[right_forks(i)]); 
	} else {
		pthread_mutex_lock(&forks[right_forks(i)]); 
		usleep(1);
		pthread_mutex_lock(&forks[left_forks(i)]); 
	}
}

void put_forks(i)
{
	pthread_mutex_unlock(&forks[right_forks(i)]);
	pthread_mutex_unlock(&forks[left_forks(i)]);
}

void *philosopher_func(void *x)
{	
	int* ptr = (int*)x;
	int id = *ptr;

	while(1) {
		think(id);
		get_forks(id);
		eat(id);
		put_forks(id);
	}

	return NULL;
}

int main(int argc, char *args[])
{
	int ids[5];
	for(int y=0; y<5; y++){
		ids[y]=y;
	}	

	for (int i=0; i<5; i++)
		pthread_mutex_init(&forks[i], NULL);
    
    for (int j=0; j<5; j++)
    	pthread_create(&philosopher[j], NULL, philosopher_func, ids+j);

	for (int k=0; k<5; k++)
    	pthread_join(philosopher[k], NULL);

    return 0;
}