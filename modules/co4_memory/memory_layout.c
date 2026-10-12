
#include <stdio.h>
#include <stdlib.h>

int global_init = 100;
int global_uninit;

void sample_function(void)
{
    printf("Inside sample function\n");
}

int main(void)
{
    int local_var = 10;
    int *heap_var = malloc(sizeof(int));

    if (heap_var == NULL)
    {
        perror("malloc");
        return 1;
    }

    *heap_var = 50;

    printf("\n=== CO-4: Process Memory Layout ===\n");
    printf("Initialized global (Data): %p\n", (void *)&global_init);
    printf("Uninitialized global (BSS): %p\n", (void *)&global_uninit);
    printf("Local variable (Stack): %p\n", (void *)&local_var);
    printf("Allocated memory (Heap): %p\n", (void *)heap_var);
    printf("Function (Code): %p\n", (void *)sample_function);

    printf("\nGlobal value: %d\n", global_init);
    printf("Local value: %d\n", local_var);
    printf("Heap value: %d\n", *heap_var);

    sample_function();

    free(heap_var);
    printf("Heap memory released successfully.\n");

    return 0;
}
