
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int local = 100;
    int *heap = malloc(sizeof(int));

    if (heap == NULL)
    {
        perror("malloc");
        return 1;
    }

    *heap = 200;

    printf("=== CO-4: Virtual Memory Demo ===\n");
    printf("Process ID: %d\n", getpid());
    printf("Virtual address of local variable: %p\n", (void *)&local);
    printf("Virtual address of heap variable:  %p\n", (void *)heap);

    printf("\nCheck this process's memory mappings using:\n");
    printf("cat /proc/%d/maps\n", getpid());

    free(heap);
    return 0;
}
