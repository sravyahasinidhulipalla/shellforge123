
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    int fd;
    char buffer[100] = {0};
    const char *message = "Hello from Linux file operations!\n";

    fd = open("sample.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    if (write(fd, message, strlen(message)) == -1)
    {
        perror("write");
        close(fd);
        return 1;
    }

    close(fd);

    fd = open("sample.txt", O_RDONLY);

    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes == -1)
    {
        perror("read");
        close(fd);
        return 1;
    }

    buffer[bytes] = '\0';

    printf("=== CO-5: Linux File Operations ===\n");
    printf("File content: %s", buffer);
    printf("Bytes read: %zd\n", bytes);

    close(fd);
    return 0;
}
