
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int n = 5;
    int *arr = malloc(n * sizeof(int));

    if (arr == NULL)
    {
        perror("malloc");
        return 1;
    }

    printf("=== CO-4: Dynamic Memory Allocation ===\n");

    for (int i = 0; i < n; i++)
        arr[i] = (i + 1) * 10;

    printf("Allocated array values:\n");

    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);

    printf("\nMemory allocated: %zu bytes\n",
           n * sizeof(int));

    free(arr);
    arr = NULL;

    printf("Memory released successfully.\n");

    return 0;
}
