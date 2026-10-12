
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(void)
{
    const char *filename = "mapped.txt";
    const char *message = "Hello from memory-mapped file I/O!\n";
    size_t length = strlen(message);

    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);

    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    if (ftruncate(fd, (off_t)length) == -1)
    {
        perror("ftruncate");
        close(fd);
        return 1;
    }

    char *data = mmap(NULL, length,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, 0);

    if (data == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        return 1;
    }

    memcpy(data, message, length);

    if (msync(data, length, MS_SYNC) == -1)
    {
        perror("msync");
        munmap(data, length);
        close(fd);
        return 1;
    }

    printf("=== CO-5: Memory-Mapped File I/O ===\n");
    printf("Mapped file content: %.*s", (int)length, data);

    if (munmap(data, length) == -1)
    {
        perror("munmap");
        close(fd);
        return 1;
    }

    if (close(fd) == -1)
    {
        perror("close");
        return 1;
    }

    printf("Memory mapping released successfully.\n");

    return 0;
}
