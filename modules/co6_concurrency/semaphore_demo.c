
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define THREADS 4

sem_t slots;

void *worker(void *arg)
{
    int id = *(int *)arg;

    sem_wait(&slots);
    printf("Thread %d: entered the resource\n", id);

    /* Simulate a short task without requiring a delay. */
    printf("Thread %d: leaving the resource\n", id);
    sem_post(&slots);

    return NULL;
}

int main(void)
{
    pthread_t threads[THREADS];
    int ids[THREADS];

    if (sem_init(&slots, 0, 2) == -1)
    {
        perror("sem_init");
        return 1;
    }

    for (int i = 0; i < THREADS; i++)
    {
        ids[i] = i + 1;

        if (pthread_create(&threads[i], NULL, worker, &ids[i]) != 0)
        {
            fprintf(stderr, "Thread creation failed\n");

            for (int j = 0; j < i; j++)
                pthread_join(threads[j], NULL);

            sem_destroy(&slots);
            return 1;
        }
    }

    for (int i = 0; i < THREADS; i++)
        pthread_join(threads[i], NULL);

    sem_destroy(&slots);
    printf("Semaphore demonstration completed.\n");

    return 0;
}
