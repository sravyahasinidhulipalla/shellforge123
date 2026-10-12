#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "ipc.h"

#define WORKER_COUNT 3
#define MESSAGE_SIZE 100

void pipe_demo(void)
{
    int fd[2];

    if (pipe(fd) == -1)
    {
        perror("pipe");
        return;
    }

    printf("\n[IPC] Creating workers for pipe communication...\n");

    for (int i = 0; i < WORKER_COUNT; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork");
            return;
        }

        if (pid == 0)
        {
            close(fd[0]);

            char message[MESSAGE_SIZE] = {0};

            snprintf(
                message,
                sizeof(message),
                "Message from Worker %d (PID %d)",
                i + 1,
                getpid()
            );

            write(fd[1], message, MESSAGE_SIZE);

            close(fd[1]);
            _exit(0);
        }
    }

    close(fd[1]);

    printf("[IPC] Parent waiting for worker messages...\n");

    for (int i = 0; i < WORKER_COUNT; i++)
    {
        char buffer[MESSAGE_SIZE] = {0};

        ssize_t bytes = read(fd[0], buffer, MESSAGE_SIZE);

        if (bytes > 0)
        {
            printf("[IPC] Parent received: %s\n", buffer);
        }
    }

    close(fd[0]);

    for (int i = 0; i < WORKER_COUNT; i++)
    {
        wait(NULL);
    }

    printf("[IPC] All workers synchronized\n");
    printf("[IPC] Anonymous pipe communication completed\n");
}

void fifo_demo(void)
{
    const char *fifo = "logs/event_fifo";

    unlink(fifo);

    if (mkfifo(fifo, 0666) == -1)
    {
        perror("mkfifo");
        return;
    }

    printf("\n[FIFO] Named pipe created: %s\n", fifo);

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        unlink(fifo);
        return;
    }

    if (pid == 0)
    {
        int fd = open(fifo, O_RDONLY);

        if (fd == -1)
        {
            perror("open");
            _exit(1);
        }

        char buffer[100] = {0};

        read(fd, buffer, sizeof(buffer) - 1);

        printf("[FIFO] Receiver PID: %d\n", getpid());
        printf("[FIFO] Received: %s\n", buffer);

        close(fd);
        _exit(0);
    }

    sleep(1);

    int fd = open(fifo, O_WRONLY);

    if (fd == -1)
    {
        perror("open");
        unlink(fifo);
        return;
    }

    const char *message = "Event message through named FIFO";

    write(fd, message, strlen(message) + 1);

    printf("[FIFO] Parent sent: %s\n", message);

    close(fd);

    waitpid(pid, NULL, 0);

    unlink(fifo);

    printf("[FIFO] Named pipe communication completed\n");
}
