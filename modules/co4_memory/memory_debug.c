
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *arr = malloc(3 * sizeof(int));

    if (arr == NULL)
    {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < 3; i++)
        arr[i] = (i + 1) * 10;

    printf("=== CO-4: Memory Debugging ===\n");

    for (int i = 0; i < 3; i++)
        printf("arr[%d] = %d\n", i, arr[i]);

    free(arr);
    arr = NULL;

    printf("Memory freed safely.\n");

    return 0;
}
