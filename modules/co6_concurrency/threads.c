
#include <stdio.h>
#include <pthread.h>

void *task(void *arg)
{
    int id = *(int *)arg;

    for (int i = 1; i <= 5; i++)
        printf("Thread %d: %d\n", id, i);

    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    if (pthread_create(&t1, NULL, task, &id1) != 0)
    {
        fprintf(stderr, "Failed to create thread 1\n");
        return 1;
    }

    if (pthread_create(&t2, NULL, task, &id2) != 0)
    {
        fprintf(stderr, "Failed to create thread 2\n");
        pthread_join(t1, NULL);
        return 1;
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Both threads completed.\n");
    return 0;
}
