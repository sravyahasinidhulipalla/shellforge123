
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    FILE *fp = fopen("buffered.txt", "w");

    if (fp == NULL)
    {
        perror("fopen");
        return 1;
    }

    fprintf(fp, "This is buffered I/O.\n");

    if (fclose(fp) == EOF)
    {
        perror("fclose");
        return 1;
    }

    int fd = open("unbuffered.txt",
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    const char *msg = "This is unbuffered I/O.\n";
    size_t len = strlen(msg);
    size_t written = 0;

    while (written < len)
    {
        ssize_t n = write(fd, msg + written, len - written);

        if (n == -1)
        {
            perror("write");
            close(fd);
            return 1;
        }

        written += (size_t)n;
    }

    if (close(fd) == -1)
    {
        perror("close");
        return 1;
    }

    printf("Buffered and unbuffered I/O completed.\n");
    printf("Check buffered.txt and unbuffered.txt\n");

    return 0;
}
