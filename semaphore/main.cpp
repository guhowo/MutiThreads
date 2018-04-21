#include <iostream>
#include <pthread.h>
#include <semaphore.h>

#define SIZE 10

sem_t blank;
sem_t data;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int buf[SIZE];
int pindex = 0;
int cindex = 0;

void *producer(void *number)
{
    int i = 0;

    while(1) {
        sem_wait(&blank);
        pthread_mutex_lock(&mutex);
        buf[pindex++] = i++;
        pindex %= SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&data);
    }
}

void *consumer(void *number)
{
    int i = 0;

    while(1) {
        sem_wait(&data);
        pthread_mutex_lock(&mutex);
        printf("comsumer %d: buf[%d] = %d\n", *(int *)number, cindex, buf[cindex]);
        cindex++;
        cindex %= SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&blank);
    }
}

int main()
{
    pthread_t producer1, producer2, consumer1, consumer2;
    int number1=1, number2=2;

    sem_init(&blank, 0, SIZE);
    sem_init(&data, 0, 0);

    pthread_create(&producer1, NULL, producer, (void *)&number1);
 // pthread_create(&producer2, NULL, producer, (void *)&number2);
    pthread_create(&consumer1, NULL, consumer, (void *)&number1);
    pthread_create(&consumer2, NULL, consumer, (void *)&number2);

    pthread_join(producer1, NULL);
    pthread_join(producer2, NULL);
    pthread_join(consumer1, NULL);
    pthread_join(consumer2, NULL);

    sem_destroy(&blank);
    sem_destroy(&data);
    pthread_mutex_destroy(&mutex);

}
