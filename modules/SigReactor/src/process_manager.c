#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include "process_manager.h"

#define WORKER_COUNT 3

void process_demo(void)
{
    pid_t workers[WORKER_COUNT];

    printf("\n[PROCESS] Starting worker processes...\n");

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
            printf("[WORKER %d] PID: %d\n", i + 1, getpid());
            printf("[WORKER %d] Parent PID: %d\n", i + 1, getppid());

            sleep(2 + i);

            printf("[WORKER %d] Exiting normally\n", i + 1);
            _exit(10 + i);
        }

        workers[i] = pid;

        printf("[PARENT] Worker %d created with PID: %d\n",
               i + 1, pid);
    }

    printf("\n[PROCESS] Monitoring workers...\n");

    for (int i = 0; i < WORKER_COUNT; i++)
    {
        int status;

        pid_t result = waitpid(workers[i], &status, 0);

        if (result > 0)
        {
            if (WIFEXITED(status))
            {
                printf("[PARENT] Worker PID %d terminated\n", result);
                printf("[PARENT] Exit status: %d\n",
                       WEXITSTATUS(status));
            }
        }
    }

    printf("[PROCESS] All workers synchronized successfully\n");
}

void process_group_demo(void)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        setpgid(0, 0);

        printf("\n[GROUP] Child PID: %d\n", getpid());
        printf("[GROUP] Process Group ID: %d\n", getpgrp());

        sleep(2);
        _exit(0);
    }

    sleep(1);

    printf("\n[GROUP] Parent PID: %d\n", getpid());
    printf("[GROUP] Child PID: %d\n", pid);
    printf("[GROUP] Sending SIGTERM to child process group\n");

    kill(-pid, SIGTERM);

    waitpid(pid, NULL, 0);

    printf("[GROUP] Process group operation completed\n");
}

void signal_worker_demo(void)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        printf("\n[WORKER] SIGCHLD demo child started\n");
        printf("[WORKER] Child PID: %d\n", getpid());
        printf("[WORKER] Child will exit after 2 seconds\n");

        sleep(2);

        printf("[WORKER] Child exiting now\n");
        _exit(0);
    }

    printf("\n[SIGCHLD] Worker created with PID: %d\n", pid);
    printf("[SIGCHLD] Parent will NOT wait immediately\n");
    printf("[SIGCHLD] Reactor will receive the child termination event\n");
}
