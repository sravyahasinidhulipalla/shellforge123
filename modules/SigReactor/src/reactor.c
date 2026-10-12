
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>

#include "reactor.h"

static int reactor_pipe[2] = {-1, -1};

static pid_t tracked_worker_pid = -1;
static int tracked_worker_done = 0;
static int tracked_worker_status = 0;
static char tracked_worker_label[128] = "";

int reactor_init(void)
{
    if (pipe(reactor_pipe) == -1)
        return -1;

    int flags = fcntl(reactor_pipe[0], F_GETFL, 0);

    if (flags == -1)
        return -1;

    if (fcntl(reactor_pipe[0], F_SETFL, flags | O_NONBLOCK) == -1)
        return -1;

    printf("[REACTOR] Event channel created\n");
    return 0;
}

int reactor_get_fd(void)
{
    return reactor_pipe[1];
}

void reactor_run_once(void)
{
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(reactor_pipe[0], &readfds);

    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    int result = select(
        reactor_pipe[0] + 1,
        &readfds,
        NULL,
        NULL,
        &tv
    );

    if (result == -1)
    {
        if (errno != EINTR)
            perror("[REACTOR] select");
        return;
    }

    if (result > 0 && FD_ISSET(reactor_pipe[0], &readfds))
    {
        char event;

        while (read(reactor_pipe[0], &event, 1) == 1)
        {
            printf("\n[REACTOR] Event received\n");

            if (event == SIGUSR1)
            {
                printf("[EVENT] SIGUSR1 -> User event handled\n");
            }
            else if (event == SIGUSR2)
            {
                printf("[EVENT] SIGUSR2 -> User event handled\n");
            }
            else if (event == SIGCHLD)
            {
                printf("[EVENT] SIGCHLD -> Child process event\n");

                int status;
                pid_t child_pid;

                while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0)
                {
                    printf(
                        "[PROCESS] Child PID %ld cleaned by reactor\n",
                        (long)child_pid
                    );

                    if (child_pid == tracked_worker_pid)
                    {
                        tracked_worker_status = status;
                        tracked_worker_done = 1;

                        if (WIFEXITED(status))
                        {
                            printf(
                                "[EVENT] Demo '%s' completed; exit status: %d\n",
                                tracked_worker_label,
                                WEXITSTATUS(status)
                            );
                        }
                        else if (WIFSIGNALED(status))
                        {
                            printf(
                                "[EVENT] Demo '%s' terminated by signal %d\n",
                                tracked_worker_label,
                                WTERMSIG(status)
                            );
                        }
                    }
                }
            }
            else if (event == SIGINT)
            {
                printf("[EVENT] SIGINT -> Shutdown event\n");
            }

            printf("[REACTOR] Event dispatched successfully\n");
        }

        if (errno != EAGAIN && errno != EWOULDBLOCK)
            perror("[REACTOR] read");
    }
}

int reactor_launch_worker(const char *path, const char *label)
{
    if (tracked_worker_pid != -1)
    {
        fprintf(stderr, "[WORKER] Another tracked worker is active\n");
        return -1;
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("[WORKER] fork");
        return -1;
    }

    if (pid == 0)
    {
        execl(path, path, (char *)NULL);
        perror("[WORKER] exec");
        _exit(127);
    }

    tracked_worker_pid = pid;
    tracked_worker_done = 0;
    tracked_worker_status = 0;

    snprintf(
        tracked_worker_label,
        sizeof(tracked_worker_label),
        "%s",
        label
    );

    printf(
        "[WORKER] Started '%s' with PID %ld\n",
        tracked_worker_label,
        (long)pid
    );

    while (!tracked_worker_done)
        reactor_run_once();

    int status = tracked_worker_status;

    tracked_worker_pid = -1;
    tracked_worker_label[0] = '\0';

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        return 0;

    return -1;
}

void reactor_close(void)
{
    if (reactor_pipe[0] != -1)
    {
        close(reactor_pipe[0]);
        reactor_pipe[0] = -1;
    }

    if (reactor_pipe[1] != -1)
    {
        close(reactor_pipe[1]);
        reactor_pipe[1] = -1;
    }
}
