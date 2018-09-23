#include <pthread.h>
#include <stdio.h>

long int i = 0;

void* threadFunction(){
	int j = 0;
    for(int k = 0; k < 50000000; k++){
        i++;
        j++;
    }

    printf("j = %d & i = %ld\n", j, i);

    return NULL;
}

int main()
{
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, threadFunction, NULL);
    pthread_create(&thread2, NULL, threadFunction, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("The magic number is: %ld\n", i);

	return 0;
}

/*
pthread_t is used to identify a thread.
pthread_create is used to create a new thread.
pthread_join suspends execution of the calling thread until the target thread terminates.

*/