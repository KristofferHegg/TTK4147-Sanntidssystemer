#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

long int i = 0;
//pthread_mutex_t lock; //make flag
sem_t semaphore;

void* threadFunction(){
    int k = 0;
    for (int j = 0; j < 50000000; j++) {
    //pthread_mutex_lock(&lock); //give flag
    sem_wait(&semaphore);
    i++;
    sem_post(&semaphore);
    //pthread_mutex_unlock(&lock); // return flag
    k++;
    }
    printf("k = %d\n", k);
    return NULL;
}

int main(){
    sem_init(&semaphore, 0, 1);
    pthread_t thread1, thread2;
    
    pthread_create(&thread1, NULL, threadFunction, NULL);
    pthread_create(&thread2, NULL, threadFunction, NULL);
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    //pthread_mutex_destroy(&lock); //destroy flag
    sem_destroy(&semaphore);
    
    printf("The magic number is: %ld\n", i);
    return 0;
}

/*
- To initialize a semaphore, use sem_init(&sem_name, pshared, int_value). Sem_name points to a semaphore object to initialize. 
  Value is an initial value to set the semaphore to.
- To wait on a semaphore, use sem_wait(&sem_name).
- To increment the value of a semaphore, use sem_post(&sem_name).
*/