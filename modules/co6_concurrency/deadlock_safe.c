
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t first = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t second = PTHREAD_MUTEX_INITIALIZER;

void *worker(void *arg)
{
    int id = *(int *)arg;

    pthread_mutex_lock(&first);
    printf("Thread %d: locked first resource\n", id);

    pthread_mutex_lock(&second);
    printf("Thread %d: locked second resource\n", id);

    printf("Thread %d: completed safely\n", id);

    pthread_mutex_unlock(&second);
    pthread_mutex_unlock(&first);

    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    int a = 1, b = 2;

    pthread_create(&t1, NULL, worker, &a);
    pthread_create(&t2, NULL, worker, &b);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&first);
    pthread_mutex_destroy(&second);

    printf("Deadlock prevention demo completed.\n");
    return 0;
}
