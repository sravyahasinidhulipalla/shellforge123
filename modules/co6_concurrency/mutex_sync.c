
#include <stdio.h>
#include <pthread.h>

#define ITERATIONS 100000

long counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *increment(void *arg)
{
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++)
    {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

int main(void)
{
    pthread_t t1, t2;

    if (pthread_create(&t1, NULL, increment, NULL) != 0)
    {
        fprintf(stderr, "Failed to create thread 1\n");
        return 1;
    }

    if (pthread_create(&t2, NULL, increment, NULL) != 0)
    {
        fprintf(stderr, "Failed to create thread 2\n");
        pthread_join(t1, NULL);
        return 1;
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Expected counter: %d\n", 2 * ITERATIONS);
    printf("Actual counter:   %ld\n", counter);

    pthread_mutex_destroy(&lock);
    return 0;
}
