
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>

int main(void)
{
    size_t size = 16 * 1024 * 1024;
    long page_size = sysconf(_SC_PAGESIZE);

    if (page_size <= 0)
    {
        fprintf(stderr, "Unable to determine page size\n");
        return 1;
    }

    unsigned char *memory = mmap(
        NULL, size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1, 0
    );

    if (memory == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    struct rusage before, after;

    if (getrusage(RUSAGE_SELF, &before) == -1)
    {
        perror("getrusage");
        munmap(memory, size);
        return 1;
    }

    volatile unsigned long total = 0;

    for (size_t i = 0; i < size; i += (size_t)page_size)
    {
        memory[i] = 1;
        total += memory[i];
    }

    if (getrusage(RUSAGE_SELF, &after) == -1)
    {
        perror("getrusage");
        munmap(memory, size);
        return 1;
    }

    printf("=== CO-4: Demand Paging Demo ===\n");
    printf("Allocated region: %zu bytes\n", size);
    printf("System page size: %ld bytes\n", page_size);
    printf("Pages touched: %zu\n",
           (size + (size_t)page_size - 1) / (size_t)page_size);
    printf("Minor page faults during access: %ld\n",
           after.ru_minflt - before.ru_minflt);
    printf("Major page faults during access: %ld\n",
           after.ru_majflt - before.ru_majflt);
    printf("Access checksum: %lu\n", total);

    if (munmap(memory, size) == -1)
    {
        perror("munmap");
        return 1;
    }

    printf("Memory mapping released successfully.\n");
    return 0;
}
