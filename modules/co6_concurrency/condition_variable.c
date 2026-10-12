
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
int ready = 0;

void *worker(void *arg)
{
    (void)arg;

    pthread_mutex_lock(&mutex);

    while (!ready)
        pthread_cond_wait(&condition, &mutex);

    printf("Worker: condition received, proceeding.\n");

    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(void)
{
    pthread_t thread;

    if (pthread_create(&thread, NULL, worker, NULL) != 0)
    {
        fprintf(stderr, "Thread creation failed.\n");
        return 1;
    }

    pthread_mutex_lock(&mutex);
    ready = 1;
    printf("Main: signaling the worker.\n");
    pthread_cond_signal(&condition);
    pthread_mutex_unlock(&mutex);

    pthread_join(thread, NULL);

    pthread_cond_destroy(&condition);
    pthread_mutex_destroy(&mutex);

    printf("Condition-variable demo completed.\n");
    return 0;
}
