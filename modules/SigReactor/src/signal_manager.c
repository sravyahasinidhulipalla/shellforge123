#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include "signal_manager.h"

static int event_fd = -1;

static void signal_handler(int sig)
{
    char event = (char)sig;
    if (event_fd != -1)
        write(event_fd, &event, 1);
}

int signal_manager_init(int fd)
{
    event_fd = fd;

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) return -1;
    if (sigaction(SIGUSR2, &sa, NULL) == -1) return -1;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) return -1;
    if (sigaction(SIGINT, &sa, NULL) == -1) return -1;

    printf("[SIGNAL] Signal handlers registered\n");
    printf("[SIGNAL] SIGUSR1  -> User event 1\n");
    printf("[SIGNAL] SIGUSR2  -> User event 2\n");
    printf("[SIGNAL] SIGCHLD  -> Child event\n");
    printf("[SIGNAL] SIGINT   -> Shutdown event\n");

    return 0;
}

void signal_manager_cleanup(void)
{
    event_fd = -1;
}
